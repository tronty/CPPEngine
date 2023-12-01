var SekindoClientDetections_GDPR = function (isDebug, globalTimeout, onConsentAvail, isApp) {

	this.CONSENT_SDK_NOT_AVAILABLE = -100;
	this.CONSENT_STILL_RUNNING = -2;
	this.CONSENT_REJECTED = 0;
	this.CONSENT_APPROVED = 1;
	this.CONSENT_NOT_APPLY = 2;
	this.CONSENT_ANOTHER_VER = 3;

	this.PRIVACY_DETECTION_TIMEOUT_STEP_MS = 50;


	this.VENDOR_ID = 228;
	this.PURPOSES = [1,3,4,5];

	this.isCmpLoaded = false;
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

	this.cmpCallsList = {};

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

	this._postMessage = function (cb, command)
	{
		try
		{
			window.addEventListener('message', cb);

			var tempTs = new Date().getTime();
			var callId = "PrimisCmpCall_"+command+"_"+Math.random().toString()+"_"+tempTs.toString();
			this.cmpCallsList[callId] = command;
			var message = {
				__cmpCall: {
					callId: callId,
					command: command
				}
			};
			if (this.debug)
				console.log("SEKDBG/GDPR: Sending message '"+command+"' to CMP");
			window.top.postMessage(message, '*');
			return true;
		}
		catch (e)
		{
			if (this.debug)
			{
				console.log("SEKDBG/GDPR: Failed to send message '"+command+"' to CMP");
				console.log(e);
			}
			return false;
		};
	};

	this._call = function (command)
	{
		var ref = this;

		if (this.consentState != this.CONSENT_STILL_RUNNING || !this.cmpCallbacksList.hasOwnProperty(command) ||
			typeof this.cmpCallbacksList[command] !== 'function')
		{
			this.cmpCallsList = {};
			return;
		}

		var currTs = new Date().getTime();
		if (currTs - ref.startTs >= this.requestTimeout) /* Timeout reached */
		{
			this.cmpCallsList = {};
			return;
		}

		if (this.cmpStopCallbacksList.hasOwnProperty(command) && typeof this.cmpStopCallbacksList[command] === 'function' && this.cmpStopCallbacksList[command]())
		{
			if (this.debug)
				console.log("SEKDBG/GDPR: Stopping "+command+" due to reached goals");
			return;
		}

		try
		{
			window.top.__cmp(command, null, this.cmpCallbacksList[command]);
			if (ref.debug)
				console.log("SEKDBG/GDPR: Using DIRECT/"+command);
		}
		catch (e)
		{
			/* Trap to check if another version is supported */
			try
			{
				var otherVer = window.top.__tcfapi;
				this.consentState = this.CONSENT_ANOTHER_VER;
				if (ref.debug)
					console.log("SEKDBG/GDPR: Skipping due to another version");
				return;
			}
			catch (eSf)
			{

			}

			try
			{
				var safeFrameHandler = window.extern || window.$sf.ext;
				safeFrameHandler.ext.register(300, 250, function (msgCat, msg) {
					if (msgCat !== 'cmpReturn')
						return;

					if (ref.cmpCallbacksList.hasOwnProperty(msg.cmpCommand) && typeof ref.cmpCallbacksList[msg.cmpCommand] === 'function')
						ref.cmpCallbacksList[msg.cmpCommand](msg.VendorConsents);
				});

				safeFrameHandler.ext.cmp(command);
				if (this.debug)
					console.log("SEKDBG/GDPR: Using SAFE_FRAME/"+command);
			}
			catch (eSf)
			{
				if (this.debug)
					console.log("SEKDBG/GDPR: Using MESSAGING/"+command);
				this._postMessage(function (evt) {
					if (evt && evt.data && evt.data.__cmpReturn && evt.data.__cmpReturn.returnValue && evt.data.__cmpReturn.callId)
					{
						if (ref.debug)
							console.log("SEKDBG/GDPR: Received message '"+evt.data.__cmpReturn.callId+"' from channel");
						if (ref.cmpCallsList.hasOwnProperty(evt.data.__cmpReturn.callId))
						{
							if (typeof ref.cmpCallsList[evt.data.__cmpReturn.callId] === 'string')
							{
								var cmd = ref.cmpCallsList[evt.data.__cmpReturn.callId];
								if (ref.cmpCallbacksList.hasOwnProperty(cmd) && typeof ref.cmpCallbacksList[cmd] === 'function')
									ref.cmpCallbacksList[cmd](evt.data.__cmpReturn.returnValue);
							}
							delete ref.cmpCallsList[evt.data.__cmpReturn.callId];
						}
					}
					else if (ref.debug && evt && evt.data && evt.data.__cmpReturn)
					{
						console.log("SEKDBG/GDPR: Received corrupted message from channel");
						console.log(evt);
					}
				}, command);
			}
		}

		if (this.consentState == this.CONSENT_STILL_RUNNING && currTs - this.startTs < this.requestTimeout)
		{
			if (!this.cmpStopCallbacksList.hasOwnProperty(command) || typeof this.cmpStopCallbacksList[command] !== 'function' ||
				!this.cmpStopCallbacksList[command]())
			{
				setTimeout(function () {
					ref._call(command);
				}, this.PRIVACY_DETECTION_TIMEOUT_STEP_MS);
			}
		}
		else
		{
			this.cmpCallsList = {};
		}
	};

	this._verify = function (rawConsent)
	{
		if (this.consentState != this.CONSENT_STILL_RUNNING) /* Already finished before ... */
			return;

		if (typeof rawConsent.gdprApplies !== 'undefined' && !rawConsent.gdprApplies)
		{
			if (this.debug)
				console.log("SEKDBG/GDPR: Consent SDK checked that GDPR does not apply for this client");
			this.consentState = this.CONSENT_NOT_APPLY;
			return true;
		}

		if (typeof rawConsent.consentData !== 'string')
		{
			if (this.debug)
			{
				console.log("SEKDBG/GDPR: Bad consent data is provided");
				console.log(rawConsent.consentData);
				console.log(rawConsent);
			}
			this.consentState = this.CONSENT_SDK_NOT_AVAILABLE;
			return false;
		}

		if (!SekindoConsentHandler || !SekindoConsentHandler.ConsentString)
		{
			if (this.debug)
				console.log("SEKDBG/GDPR: IAB Consent SDK is not available");
			this.consentState = this.CONSENT_SDK_NOT_AVAILABLE;
			return false;
		}

		if (this.debug)
		{
			console.log("SEKDBG/GDPR: Consent returned by SDK:");
			console.log(rawConsent);
		}

		var consentHandler = new SekindoConsentHandler.ConsentString(rawConsent.consentData);
		var acceptedVendors = consentHandler.getVendorsAllowed();
		if (!acceptedVendors || acceptedVendors.indexOf(this.VENDOR_ID) == -1)
		{
			if (this.debug)
				console.log("SEKDBG/GDPR: Vendor rejected by client");
			this.consentState = this.CONSENT_REJECTED;
			this.consentRawData = rawConsent.consentData;
			return false;
		}

		for (i = 0, len = this.PURPOSES.length; i < len; i ++)
		{
			if (!consentHandler.isPurposeAllowed(this.PURPOSES[i]))
			{
				if (this.debug)
					console.log("SEKDBG/GDPR: Purpose "+this.PURPOSES[i]+" rejected by client");
				this.consentState = this.CONSENT_REJECTED;
				this.consentRawData = rawConsent.consentData;
				return false;
			}
		}

		this.consentState = this.CONSENT_APPROVED;
		this.consentRawData = rawConsent.consentData;

		if (this.debug)
			console.log("SEKDBG/GDPR: Vendor approval received");

		return true;
	};

	var ref = this;

	this.cmpCallbacksList = {
		ping: function (result) {
				if (typeof result.cmpLoaded !== 'undefined' && result.cmpLoaded)
				{
					ref.isCmpLoaded = true;
					if (ref.debug)
						console.log("SEKDBG/GDPR: CMP library is now fully loaded");
					if (typeof ref.onCmpLoaded === 'function')
						ref.onCmpLoaded();
				}
		},
		getConsentData: function (result) {
				ref._verify(result);

				/* On success/non-required, call the availability callback */
				if (ref.onConsentAvail && (ref.consentState == ref.CONSENT_APPROVED || ref.consentState == ref.CONSENT_REJECTED))
					ref.onConsentAvail(ref.consentRawData, ref.consentState == ref.CONSENT_APPROVED);
		}
	};

	this.cmpStopCallbacksList = {
		ping: function () {
			return ref.isCmpLoaded;
		}
	};

	this.onCmpLoaded = function () {
		this._call('getConsentData');
	};

	/* Track CMP state and fetch data then ready */
	if (!this.isApp) {
		this._call('ping');
	}
};