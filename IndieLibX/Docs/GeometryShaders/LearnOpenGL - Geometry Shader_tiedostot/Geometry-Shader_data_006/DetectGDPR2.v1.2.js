/* CMP v2 Implementation - TCF 2.2 */
var SekindoClientDetections_GDPR_v2 = function (isDebug, globalTimeout, onConsentAvail, isApp) {
	this.CONSENT_SDK_NOT_AVAILABLE = -100;
	this.CONSENT_STILL_RUNNING = -2;
	this.CONSENT_REJECTED = 0;
	this.CONSENT_APPROVED = 1;
	this.CONSENT_NOT_APPLY = 2;
	this.CONSENT_ANOTHER_VER = 3;

	this.VERSION = 2;

	this.PRIVACY_DETECTION_TIMEOUT_STEP_MS = 50;

	this.VENDOR_ID = 228;
	this.PURPOSES = [1,2,5,6,7,8];

	this.isCmpLoaded = false;
	this.cmpVersion = '';
	this.consentState = this.CONSENT_STILL_RUNNING;
	this.consentRawData = null;
	this.debug = isDebug;
	this.requestTimeout = globalTimeout;
	this.isApp = isApp;

	this.startTs = new Date().getTime();

	if (onConsentAvail && typeof onConsentAvail == 'function')
		this.onConsentAvail = onConsentAvail;
	else
		this.onConsentAvail = null;

	this.tcfCallsList = {};

	if (typeof window.Sekindo == 'undefined')
		window.Sekindo = {};
	if (typeof window.Sekindo.clientConsentEncoded == 'undefined')
		window.Sekindo.clientConsentEncoded = null;

	this.getState = function()
	{
		return this.consentState;
	};

	this.getConsent = function()
	{
		return this.consentRawData ? this.consentRawData : '';
	};

	this._postMessage = function (cb, command, param)
	{
		try
		{
			window.addEventListener('message', cb);

			var tempTs = new Date().getTime();
			var callId = "PrimisCmp2Call_"+command+"_"+Math.random().toString()+"_"+tempTs.toString();
			this.tcfCallsList[callId] = command;
			var message = {
				__tcfapiCall: {
					callId: callId,
					command: command,
					parameter: param,
					version: this.VERSION
				}
			};
			if (this.debug)
				console.log("SEKDBG/GDPRv2: Sending message '"+command+"' to CMP");
			window.top.postMessage(message, '*');
			return true;
		}
		catch (e)
		{
			if (this.debug)
			{
				console.log("SEKDBG/GDPRv2: Failed to send message '"+command+"' to CMP");
				console.log(e);
			}
			return false;
		};
	};

	this._call = function (command, param)
	{
		var ref = this;
		const isEventListenerCmd = (command == 'addEventListener' || command == 'removeEventListener');
		if ((this.consentState != this.CONSENT_STILL_RUNNING || !this.tcfCallbacksList.hasOwnProperty(command) ||
			typeof this.tcfCallbacksList[command] !== 'function') && !isEventListenerCmd)
		{
			this.tcfCallsList = {};
			return;
		}

		var currTs = new Date().getTime();
		if (currTs - ref.startTs >= this.requestTimeout && !isEventListenerCmd) /* Timeout reached */
		{
			this.tcfCallsList = {};
			return;
		}

		if (this.tcfStopCallbacksList.hasOwnProperty(command) && typeof this.tcfStopCallbacksList[command] === 'function'
			&& this.tcfStopCallbacksList[command]() && !isEventListenerCmd)
		{
			if (this.debug)
				console.log("SEKDBG/GDPRv2: Stopping "+command+" due to reached goals");
			return;
		}

		try
		{
			window.top.__tcfapi(command, this.VERSION, this.tcfCallbacksList[command], param);
			if (ref.debug)
				console.log("SEKDBG/GDPRv2: Using DIRECT/"+command);
		}
		catch (e)
		{
			/* Trap to check if another version is supported */
			try
			{
				var otherVer = window.top.__cmp;
				this.consentState = this.CONSENT_ANOTHER_VER;
				if (ref.debug)
					console.log("SEKDBG/GDPRv2: Skipping due to another version");
				return;
			}
			catch (eSf)
			{

			}

			try
			{
				var safeFrameHandler = window.extern || window.$sf.ext;
				safeFrameHandler.ext.register(300, 250, function (msgCat, msg) {
					if (msgCat !== 'tcfapiReturn')
						return;

					if (ref.tcfCallbacksList.hasOwnProperty(msg.cmpCommand) && typeof ref.tcfCallbacksList[msg.cmpCommand] === 'function')
						ref.tcfCallbacksList[msg.cmpCommand](msg.VendorConsents);
				});

				safeFrameHandler.ext.tcfapi(command, param);
				if (this.debug)
					console.log("SEKDBG/GDPRv2: Using SAFE_FRAME/"+command);
			}
			catch (eSf)
			{
				if (this.debug)
					console.log("SEKDBG/GDPRv2: Using MESSAGING/"+command);
				this._postMessage(function (evt) {
					if (evt && evt.data && evt.data.__tcfapiReturn && evt.data.__tcfapiReturn.returnValue && evt.data.__tcfapiReturn.callId)
					{
						if (ref.debug)
							console.log("SEKDBG/GDPRv2: Received message '"+evt.data.__tcfapiReturn.callId+"' from channel");
						if (ref.tcfCallsList.hasOwnProperty(evt.data.__tcfapiReturn.callId))
						{
							if (typeof ref.tcfCallsList[evt.data.__tcfapiReturn.callId] === 'string')
							{
								var cmd = ref.tcfCallsList[evt.data.__tcfapiReturn.callId];
								if (ref.tcfCallbacksList.hasOwnProperty(cmd) && typeof ref.tcfCallbacksList[cmd] === 'function')
									ref.tcfCallbacksList[cmd](evt.data.__tcfapiReturn.returnValue);
							}
							delete ref.tcfCallsList[evt.data.__tcfapiReturn.callId];
						}
					}
					else if (ref.debug && evt && evt.data && evt.data.__tcfapiReturn)
					{
						console.log("SEKDBG/GDPRv2: Received corrupted message from channel");
						console.log(evt);
					}
				}, command, param);
			}
		}

		if (this.consentState == this.CONSENT_STILL_RUNNING && currTs - this.startTs < this.requestTimeout && !isEventListenerCmd)
		{
			if (!this.tcfStopCallbacksList.hasOwnProperty(command) || typeof this.tcfStopCallbacksList[command] !== 'function' ||
				!this.tcfStopCallbacksList[command]())
			{
				setTimeout(function () {
					ref._call(command);
				}, this.PRIVACY_DETECTION_TIMEOUT_STEP_MS);
			}
		}
		else
		{
			this.tcfCallsList = {};
		}
	};

	this._verify = function (rawConsent, isUpdate)
	{
		if (this.consentState != this.CONSENT_STILL_RUNNING && !isUpdate) /* Already finished before ... */
			return;

		if (typeof rawConsent.gdprApplies !== 'undefined' && !rawConsent.gdprApplies)
		{
			if (this.debug)
				console.log("SEKDBG/GDPRv2: Consent SDK checked that GDPR does not apply for this client");
			this.consentState = this.CONSENT_NOT_APPLY;
			return true;
		}

		if (typeof rawConsent.tcString !== 'string')
		{
			if (this.debug)
			{
				console.log("SEKDBG/GDPRv2: Bad consent data is provided");
				console.log(rawConsent.tcString);
				console.log(rawConsent);
			}
			this.consentState = this.CONSENT_SDK_NOT_AVAILABLE;
			return false;
		}

		if (this.debug)
		{
			console.log("SEKDBG/GDPRv2: Consent returned by SDK:");
			console.log(rawConsent);
		}

		if (rawConsent.vendor.consents[this.VENDOR_ID] === undefined || !rawConsent.vendor.consents[this.VENDOR_ID])
		{
			if (this.debug)
				console.log("SEKDBG/GDPRv2: Vendor rejected by client");
			this.consentState = this.CONSENT_REJECTED;
			this.consentRawData = rawConsent.tcString;
			return false;
		}

		for (i = 0, len = this.PURPOSES.length; i < len; i ++)
		{
			if (rawConsent.purpose.consents[this.PURPOSES[i]] === undefined || !rawConsent.purpose.consents[this.PURPOSES[i]])
			{
				if (this.debug)
					console.log("SEKDBG/GDPRv2: Purpose "+this.PURPOSES[i]+" rejected by client");
				this.consentState = this.CONSENT_REJECTED;
				this.consentRawData = rawConsent.tcString;
				return false;
			}
		}

		this.consentState = this.CONSENT_APPROVED;
		this.consentRawData = rawConsent.tcString;

		if (this.debug)
			console.log("SEKDBG/GDPRv2: Vendor approval received");

		return true;
	};

	var ref = this;

	this.tcfCallbacksList = {
		ping: function (result) {
				if (typeof result.cmpLoaded !== 'undefined' && result.cmpLoaded)
				{
					ref.isCmpLoaded = true;
					if (result.cmpVersion !== undefined)
						ref.cmpVersion = result.cmpVersion;
					if (ref.debug)
						console.log("SEKDBG/GDPRv2: CMP library is now fully loaded");
					if (typeof ref.onCmpLoaded === 'function')
						ref.onCmpLoaded();
				}
		},
		addEventListener: function (result, isSuccess) {
			if (isSuccess && (result.eventStatus == 'useractioncomplete' || result.eventStatus == 'tcloaded')) {
				ref._verify(result, result.eventStatus == 'useractioncomplete');
				/* On success/non-required, call the availability callback */
				if (ref.onConsentAvail && (ref.consentState == ref.CONSENT_APPROVED || ref.consentState == ref.CONSENT_REJECTED)) {
					ref.onConsentAvail(ref.consentRawData, ref.consentState == ref.CONSENT_APPROVED);
					/* Not removing the event listener to allow interception of user consent change action */
				}
			}
		}
	};

	this.tcfStopCallbacksList = {
		ping: function () {
			return ref.isCmpLoaded;
		}
	};

	this.onCmpLoaded = function () {
		/* In TCF 2.2 the getTCData is deprecated, using the addEventListener only */
		this._call('addEventListener');
	};

	/* Track CMP state and fetch data then ready */
	if (!this.isApp) {
		this._call('ping');
	}
};