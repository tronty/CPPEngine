			// <script type="text/javascript">
			(function () {
				var SekindoClientDynamicConfig = function (config, prob)
{
	this.MOBILE_MAX_WIDTH = 415;
	this.config = config;
	this.prob = prob;

	var ref = this;

	this.sizeSetup = function(rules)
	{
		var selectedRule = rules[rules.length-1];
		for (var i=0; i<rules.length-1; i++)
		{
			if (rules[i][0] <= ref.prob.window.width)
			{
				selectedRule = rules[i];
				break;
			}
		}

		ref.config.unit.width = selectedRule[0];
		ref.config.unit.height = selectedRule[1];
	};

	var compareObjectsByKeys = function(obj1, obj2)
	{
		for (var key in obj1)
		{
			if (typeof obj1[key] == "object" && obj2.hasOwnProperty(key) && typeof obj2[key] == "object")
			{
				if (!compareObjectsByKeys(obj1[key],obj2[key]))
				{
					return false;
				}
				continue;
			}
			if (!obj2.hasOwnProperty(key) || obj1[key] != obj2[key])
			{
				return false;
			}
		}
		return true;
	};
	
	this.clientInfoSetup = function(rules)
	{
		var clientInfoRule = rules[0];
		if (compareObjectsByKeys(clientInfoRule, ref.prob.ci))
		{
			var configRule = rules[1];
			for (var property in configRule)
			{
				ref.config[property] = configRule[property];
			}
		}
	};

	this.deviceTypeSetup = function(rules)
	{
		var selectedRule = rules[rules.length-1];
		for (var i=0; i<rules.length-1; i++)
		{
			if (rules[i][0].toLowerCase() == ref.prob.ci.deviceType.toLowerCase())
			{
				selectedRule = rules[i];
				break;
			}
		}

		ref.config.unit.width = selectedRule[1];
		ref.config.unit.height = selectedRule[2];

		if (selectedRule[3] != 'undefined' && selectedRule[3].length > 0)
		{
			var floatSetup = selectedRule[3];
			if (floatSetup[0] === 1)
			{
				if (!ref.isStickyVideoType())
				{
					ref.config.videoType = 'flow';
				}
			}
			else
			{
				if (ref.isStickyVideoType())
				{
					ref.config.url += '&blockAlwaysSticky=1';
				}
				ref.config.videoType = 'normal';
			}

			ref.config.floatConfig.width = floatSetup[1];
			ref.config.floatConfig.height = floatSetup[2];
			ref.config.floatConfig.direction = floatSetup[3];
			ref.config.floatConfig.verticalOffset = floatSetup[4];
			ref.config.floatConfig.horizontalOffset = floatSetup[5];
			ref.config.floatConfig.isCloseBtn = floatSetup[6];
		}
	};

	this.tabletFloatSetup = function(rules)
	{
		if (rules[0].toLowerCase() == ref.prob.ci.deviceType.toLowerCase())
		{
			if (rules[1][0] === 1)
			{
				var floatSetup = rules[1];
				if (!ref.isStickyVideoType())
				{
					ref.config.videoType = 'flow';
				}
				ref.config.floatConfig.width = floatSetup[1];
				ref.config.floatConfig.height = floatSetup[2];
				ref.config.floatConfig.direction = floatSetup[3];
				ref.config.floatConfig.verticalOffset = floatSetup[4];
				ref.config.floatConfig.horizontalOffset = floatSetup[5];
				ref.config.floatConfig.isCloseBtn = floatSetup[6];
				ref.config.floatConfig.flowMode = floatSetup[7];
				ref.config.floatConfig.closeBtnPos = floatSetup[8];
			}
			else
			{
				if (ref.isStickyVideoType())
				{
					ref.config.url += '&blockAlwaysSticky=1';
				}
				ref.config.videoType = 'normal';
			}
		}
	}

	this.sliderSetup = function(rules)
	{
		var selectedRule = rules[rules.length-1];
		for (var i=0; i<rules.length-1; i++)
		{
			if (rules[i][0] <= ref.prob.window.width)
			{
				selectedRule = sizes[i];
				break;
			}
		}

		ref.config.unit.width = selectedRule[0];
		ref.config.unit.height = selectedRule[1];
	}

	this.floatSetupSetDefaults = function (selectedRule)
	{
		// Floating display timeout after close float button has clicked
		if (typeof selectedRule['flowCloseTimeout'] === 'undefined' || selectedRule['flowCloseTimeout'] === '')
		{
			selectedRule['flowCloseTimeout'] = 0;
		}
		// Floating close button position
		if (typeof selectedRule['closeBtnPos'] === 'undefined' || selectedRule['closeBtnPos'] != 'left')
		{
			selectedRule['closeBtnPos'] = 'right';
		}
		return selectedRule;
	}
	
	this.floatSetupSelectRule = function (rules)
	{
		var selectedRule = rules[rules.length-1];
		for (var i=0; i<rules.length-1; i++)
		{
			if (rules[i]['screenWidthSize'] < ref.prob.window.width)
			{
				if (typeof rules[i]['screenHeightSize'] !== 'undefined' && rules[i]['screenHeightSize'] != -1)
				{
					if (rules[i]['screenHeightSize'] < ref.prob.window.height)
					{
						selectedRule = rules[i];
						break;
					}
					else
					{
						continue;
					}
				}
				else
				{
					selectedRule = rules[i];
					break;
				}
			}
		}
		return ref.floatSetupSetDefaults(selectedRule);
	}
	
	this.floatSetup = function(rules)
	{
		var selectedRule = ref.floatSetupSelectRule(rules);
		
		if (selectedRule['enabled'] === 1)
		{
			if (!ref.isStickyVideoType())
			{
				ref.config.videoType = 'flow';
			}
		}
		else
		{
			if (ref.isStickyVideoType())
			{
				ref.config.url += '&blockAlwaysSticky=1';
			}
			ref.config.videoType = 'normal';
		}

		ref.config.floatConfig = selectedRule;
		
		// Allow floating for only given GEO code
		if (typeof selectedRule['allowFloatingGeo'] !== 'undefined' && selectedRule['allowFloatingGeo'] !== '' && selectedRule['allowFloatingGeo'].indexOf(ref.prob.geo) == -1 && selectedRule['allowFloatingGeo'].indexOf('ALL_GEO') == -1)
		{
			ref.config.videoType = 'normal';
		}

		// Prevent floating for given GEO code
		if (typeof selectedRule['disallowFloatingGeo'] !== 'undefined' && selectedRule['disallowFloatingGeo'] !== '' && selectedRule['disallowFloatingGeo'].indexOf(ref.prob.geo) != -1 && selectedRule['disallowFloatingGeo'].indexOf('NO_DISALLOW_GEO') == -1)
		{
			ref.config.videoType = 'normal';
		}

		// Prevent floating for given DMA code
		if (typeof selectedRule['dmaList'] !== 'undefined' && selectedRule['dmaList'].indexOf(ref.prob.dmaCode) != -1)
		{
			ref.config.videoType = 'normal';
		}
	}

	this.clientSideSetup = function (rules) {
		var clientSideRules = rules[0];
		var configRule = rules[1];
		for (var clientProperty in clientSideRules)
		{
			if (eval(clientProperty) == clientSideRules[clientProperty])
			{
				for (var property in configRule)
				{
					ref.config[property] = configRule[property];
				}
			}
		}
	}

	this.playerDivSetup = function(rules)
	{
		if (typeof rules.mainElement !== 'undefined' && rules.mainElement["type"] && rules.mainElement["name"])
		{
			if (rules.mainElement["type"] == "id")
			{
				ref.config.playerDivSetup.elmtId = rules.mainElement["name"];
			}
			else if (rules.mainElement["type"] == "class")
			{
				ref.config.playerDivSetup.elmtClassName = rules.mainElement["name"];
			}
			else if (rules.mainElement["type"] == "querySelector")
			{
				ref.config.playerDivSetup.querySelector = rules.mainElement["name"];
			}

			if (typeof rules.mainElementNumber !== 'undefined')
			{
				ref.config.playerDivSetup.mainElementNumber = rules.mainElementNumber;
			}

			if (typeof rules.childElement !== 'undefined')
			{
				if (rules.childElement["type"] == "id")
				{
					ref.config.playerDivSetup.childElement = {};
					ref.config.playerDivSetup.childElement.elmtId = rules.childElement["name"];
				}
				else if (rules.childElement["type"] == "class")
				{
					ref.config.playerDivSetup.childElement = {};
					ref.config.playerDivSetup.childElement.elmtClassName = rules.childElement["name"];
				}
			}

			if (typeof rules.childElementNumber !== 'undefined')
			{
				ref.config.playerDivSetup.childElementNumber = rules.childElementNumber;
			}

			if (typeof rules.childElementTagName !== 'undefined')
			{
				ref.config.playerDivSetup.childElementTagName = rules.childElementTagName;
			}

			if (typeof rules.insertPosition !== 'undefined')
			{
				ref.config.playerDivSetup.insertPosition = rules.insertPosition;
			}
		}
	}

	this.newParamMapping = function(rules)
	{
		var selectedRule = rules[rules.length-1];
		for (var i=0; i<rules.length; i++)
		{
			var tagAdd='';
			var oldParam = rules[i][0];
			var newParam = rules[i][1];

			var regex = new RegExp("(\\?|&)"+oldParam+"\\=([^&]*)");
			var oldParamValue = ref.config.url.match(regex);
			if (oldParamValue && oldParamValue[2] != undefined)
			{
				ref.config.url += '&'+newParam+'='+oldParamValue[2];
			}
		}
	}

	this.templateOverride = function(rules)
	{
		var overrideAllowed = true;
		if ((typeof rules.screenSize != 'undefined') && (typeof rules.screenSize[0] != 'undefined'))
		{
			overrideAllowed = ref.prob.window.width < rules.screenSize[0];
			if (overrideAllowed && (typeof rules.screenSize[1] != 'undefined'))
			{
				overrideAllowed = ref.prob.window.height < rules.screenSize[1];
			}
		}
		if (typeof rules.deviceType != 'undefined')
		{
			overrideAllowed = ref.prob.ci.deviceType.toLowerCase() == rules.deviceType.toLowerCase();
		}
		if (overrideAllowed && (typeof rules.newTemplateId != 'undefined'))
		{
			ref.config.vp_template = rules.newTemplateId;
		}
	}

	this.generalEmptyConfig = function (){}

	this.getDynamicConfigParams = function(i, deviceType)
	{
		var configEntity = '';
		var funcName = this.config.dynamicSetup[i][0];
		if (funcName === 'playerDivSetup' && this.config.dynamicSetup[i][1][0] && deviceType)
		{
			for (var j=0; j<this.config.dynamicSetup[i][1].length; j++)
			{
				if (this.config.dynamicSetup[i][1][j][deviceType])
				{
					configEntity = [funcName, this.config.dynamicSetup[i][1][j][deviceType]];
				}
			}
		}
		else
		{
			configEntity = [funcName, this.config.dynamicSetup[i][1]];
		}
		return configEntity;
	}

	this.run = function()
	{
		var deviceType = ref.prob.ci.deviceType.toLowerCase();
		for (var i=0; i<this.config.dynamicSetup.length; i++)
		{
			var dynamicConfigEntity = this.getDynamicConfigParams(i, deviceType);
			if (dynamicConfigEntity && dynamicConfigEntity[0] && dynamicConfigEntity[1])
			{
				try
				{
					var func = this.functionMap[dynamicConfigEntity[0]];
					func(dynamicConfigEntity[1]);
				}
				catch (e)
				{
					console.log(e.message);
				}
			}
		}
	}

	this.functionMap = {'sizeSetup' : this.sizeSetup,
						'floatSetup' : this.floatSetup,
						'deviceTypeSetup' : this.deviceTypeSetup,
						'tabletFloatSetup' : this.tabletFloatSetup,
						'playerDivSetup' : this.playerDivSetup,
						'clientInfoSetup' : this.clientInfoSetup,
						'clientSideSetup' : this.clientSideSetup,
						'newParamMapping' : this.newParamMapping,
						'templateOverride' : this.templateOverride,
						'playlistTargeting' : this.generalEmptyConfig
	};

	this.isStickyVideoType = function()
	{
		return ref.config.videoType == 'sticky';
	}
};
var SekindoClientDetections_URL = function (config) {
	/* Functions needed for constructor */
	this._getDiscoverableUrl = function()
	{
		var url = '';

		try
		{
			if (window.top == window)
			{
				url = window.location.href;
			}
			else
			{
				try
				{
					url = window.top.location.href;
				}
				catch (e2)
				{
					url = document.referrer;
				}
			}

			url = url.substring(0, 1500);
		}
		catch(e1) {}

		return url;
	}

	this._setCheckerWidget = function(allowedType)
	{
		switch (allowedType)
		{
			case "0":
				config.allowViewabilityCheck = false;
				config.allowUserScrollCheck  = false;
				break;

			case "1":
				config.allowViewabilityCheck = true;
				config.allowUserScrollCheck  = false;
				break;

			case "2":
				config.allowViewabilityCheck = false;
				config.allowUserScrollCheck  = true;
				break;
		}
	}

	this._initDebugCheck = function(config)
	{
		var debugParam, debugParamsRExp = RegExp('(\\?|&)(sekDbg_[^\=&]+)\=([^&]*)', 'gi');
		var pageUrl = this._getDiscoverableUrl();
		while ((paramMatch = debugParamsRExp.exec(pageUrl)) !== null)
		{
			debugParam = paramMatch[2].replace('sekDbg_', '');
			debugParam = debugParam.replace('sekdbg_', '');
			switch (debugParam)
			{
				case 'x':
					config.x = paramMatch[3];
					break;
				case 'y':
					config.y = paramMatch[3];
					break;
				case 'videoType':
					config.videoType = paramMatch[3];
					break;
				case 'perform':
					this._setCheckerWidget(paramMatch[3]);
					break;
				case 'log':
					config.debug = paramMatch[3];
					break;
			}
		};
	}

	/* Constructor */
	this._initDebugCheck(config);
	this.COOKIE_TIMEOUT = 7*24*60*60;
	this.VIDEO_HELPER_PARAM_NAME = 'videoHelperParam';
	this.VIDEO_SHARED_PARAM_NAME = config.sharedVideoParameterName;
	this.VIDEO_GAM_TARGETING_PARAM_NAME = config.gamTargetingVideoParameterName;

	this.floatConfig = config.floatConfig;
	this.videoType = config.videoType;
	this.frameInfo = {
		isInsideGoogleFrame: false,
		isBuildFrame: false,
		isBuildFrameViaJs : false
	};
	this.needWrappingIframe = config.needWrappingIframe;
	this.isAmpProject = config.isAmpProject;
	this.isAmpIframe = config.isAmpIframe;
	this.isAPI = config.isAPI;
	this.sizesList = config.sizesList;
	this.x = config.x;
	this.y = config.y;
	this.url = config.url;
	this.origQString = config.origQString;
	this.debug = config.debug;
	this.ci = config.ci;
	this.isFpCookie = config.isFpCookie;
	this.geo = config.geo;
	this.dmaCode = config.dmaCode;
	this.dynamicSetup = config.dynamicSetup;
	this.prob = {};
	this.uuid = config.uuid;
	this.isResponsiveBanner = config.isResponsiveBanner;
	this.hideOnMissingMainElement = config.hideOnMissingMainElement;
	this.allowedUtmParameters = config.allowedUtmParameters;
	this.placementId = config.placementId;
	this.allowDisplaySamePlacementAgain = config.allowDisplaySamePlacementAgain;
	this.blockOtherPlacements = config.blockOtherPlacements;
	this.searchByMetaTagName = config.searchMetaTagNames;
	this.customTargetingProperties = config.customTargetingProperties;
	this.customPpidMapping = config.customPpidMapping;
	this.helperParameters = {};
	this.placementType = config.placementType;
	this.isApp = config.isApp;
	this.rootWindow = null;
	this.elementChecker = {};
	this.observerElement = null;
	this.observedElementInfo   = {viewabilityThreshold:config.playerInViewPrc, isViewable:false, hasScrolled:false};
	this.allowViewabilityCheck = config.allowViewabilityCheck;
	this.allowUserScrollCheck  = config.allowUserScrollCheck;
	this.isIpad = false;
	this.isRun = config.isRun;
	this.isPlayerApiActions = config.isPlayerApiActions;

	this.startTs = new Date().getTime();
	if (this.debug)
		console.log("SEKDBG: Starting timer towards timeout");

	this.getScriptElement = function()
	{
		if (typeof this.config.playerDivSetup.elmtClassName !== 'undefined' || typeof this.config.playerDivSetup.elmtId !== 'undefined' || typeof this.config.playerDivSetup.querySelector !== 'undefined')
		{
			try
			{
				if (this.debug)
				{
					console.log("SEKDBG: playerDivSetup config");
					console.log(this.config.playerDivSetup);
				}
				var specialElmt = this._getPlayerMainElement(window.top);
				if (specialElmt && typeof specialElmt !== 'undefined')
				{
					this.srcElement = specialElmt;
					if (this.debug)
					{
						console.log("SEKDBG: playerDivSetup element");
					}
					return;
				}
				if (typeof specialElmt === 'undefined' && this.hideOnMissingMainElement)
				{
					if (this.debug)
					{
						console.log("SEKDBG: Main element is missing");
					}
					this.srcElement = null;
					return;
				}
			}
			catch (e)
			{
				if (this.debug)
				{
					console.log("SEKDBG: playerDivSetup get specialElmt error - " + e.message);
				}
			}
		}

		if (document && typeof document.currentScript !== 'undefined')
		{
			if (this.debug)
				console.log("SEKDBG: currentScript is supported");
			this.srcElement = document.currentScript;
		}
		else if (document)
		{
			if (this.debug)
				console.log("SEKDBG: currentScript is not supported");
			try
			{
				/* IE 11 and below does not support currentScript */
				var scriptsList = [];
				if (typeof document.scripts !== 'undefined' && document.scripts)
				{
					if (this.debug)
						console.log("SEKDBG: document.scripts is supported");
					scriptsList = document.scripts;
				}
				else
				{
					if (this.debug)
						console.log("SEKDBG: document.scripts is not supported");
					scriptsList = document.getElementsByTagName('script');
				}
				for (var len = scriptsList.length, i = len; i >= 0; i --)
				{
					var scriptCandidate = scriptsList[i];
					if (scriptCandidate && scriptCandidate.src && scriptCandidate.src.indexOf(this.origQString) != -1)
					{
						this.srcElement = scriptCandidate;
						break;
					}
				}
			}
			catch (e)
			{
				this.srcElement = null;
			}
		}
	}

	this._getUuid = function()
	{
		if (this.isFpCookie)
		{
			try
			{
				var uuid = window.document.cookie.replace(/(?:(?:^|.*;\s*)csuuidSekindo\s*\=\s*([^;]*).*$)|^.*$/, "$1");
				if (uuid != '')
				{
					this.uuid = uuid;
				}

				window.document.cookie = "csuuidSekindo="+ this.uuid +"; max-age=" + this.COOKIE_TIMEOUT + "; path=/";
			}
			catch (e)
			{
				this.uuid = null;
			}
		}
		else
		{
			this.uuid = null;
		}
	}

	this._handleDebugParams = function(param, value)
	{
		if (!param)
			return;
		param = param.replace('sekDbg_view_', '');
		switch (param)
		{
			case 'x':
				this.config.unit.width = value;
				break;
			case 'y':
				this.config.unit.height = value;
				break;
			case 'videoType':
				this.config.videoType = value;
				break;
			case 'flowMode':
				this.config.floatConfig.flowMode = value;
				break;
			case 'flowIsCloseBtn':
				this.config.floatConfig.isCloseBtn = value;
				break;
			case 'flowDirection':
				this.config.floatConfig.direction = value;
				break;
			case 'flowCloseButtonPosition':
				this.config.floatConfig.closeBtnPos = value;
				break;
		}
	}

    this._checkAvailCampaigns = function (pageUrl) {
        var matches = pageUrl.match(/(\?|&)availCampaigns\=([0-9,]*)([^0-9,]*)&?/i);
        if (matches && matches.length > 3 && matches[3] !== "")
            return "0";
        return (matches && matches.length > 1 && matches[2] ? matches[2] : null);
    }

	this._checkStartOverDebug = function(pageUrl)
	{
		debugIp = pageUrl.match(/(\?|&)customServerPrimis\=([^&]*)(.*)?$/i);
		if (this.isApp)
		{
			debugIp = this.url.match(/(\?|&)customServerPrimis\=([^&]*)(.*)?$/i);
		}
		if (debugIp)
		{
			this.url = this.url.replace("live.primis.tech",debugIp[2]).replace("live.sekindo.com",debugIp[2]);
			if (debugIp[3])
				this.url += debugIp[3];
		}
		debugId = pageUrl.match(/(\?|&)debugPlayerSession\=([^&]*)/i);
		if (debugId)
		{
			this.url = this.url+'&debugPlayerSession='+debugId[2];
		}

		var debugParamsRExp = RegExp('(\\?|&)(sekDbg_[^\=&]+)\=([^&]*)', 'g');
		while ((paramMatch = debugParamsRExp.exec(pageUrl)) !== null)
		{
			this.url = this.url+'&'+paramMatch[2]+'='+paramMatch[3];
			this._handleDebugParams(paramMatch[2], paramMatch[3]);
		};
	}

	this.setInfo = function()
	{
		var pageUrl = this._getDiscoverableUrl();

		this._checkStartOverDebug(pageUrl);

		if (this.config.unit.width == 0)
			return;

		var availCampaigns = this._checkAvailCampaigns(pageUrl);
		if (availCampaigns)
			this.url += '&availCampaigns=' + availCampaigns;

		if (this.isAmpProject || this.isAmpIframe) {
			this.url += '&pubUrlAuto=';// + encodeURIComponent(pageUrl);
		} else {
			this.url += '&pubUrlAuto=' + encodeURIComponent(pageUrl);
		}
		this.url = this.url.replace('SEKXLEN', this.config.unit.width);
		this.url = this.url.replace('SEKYLEN', this.config.unit.height);
		this.url = this.url.replace("'", '%27');

		this._getUuid();
		if (this.uuid != null)
		{
			this.url += '&csuuid=' + encodeURIComponent(this.uuid);
		}
		
		if (this.config.placementConfig)
		{
			for (var property in this.config.placementConfig)
			{
				this.url += '&' + property + '=' + this.config.placementConfig[property];
			}
		}

		if (this.config.videoType == 'normal')
		{
			this.url += '&videoType=normal';
		}
		else if (this.config.videoType == 'flow' || this.config.videoType == 'sticky')
		{
			this.url += '&videoType=' + this.config.videoType + '&floatWidth=' + this.config.floatConfig.width + '&floatHeight=' + this.config.floatConfig.height +
				'&floatDirection=' + this.config.floatConfig.direction + '&floatVerticalOffset=' + this.config.floatConfig.verticalOffset + '&floatHorizontalOffset=' + this.config.floatConfig.horizontalOffset +
				'&floatCloseBtn=' + this.config.floatConfig.isCloseBtn + '&flowMode='+this.config.floatConfig.flowMode + '&flowCloseButtonPosition=' + this.config.floatConfig.closeBtnPos;
		}

		if (typeof this.config.vp_template != 'undefined')
		{
			this._setReplaceUrlParam('vp_template', this.config.vp_template);
		}

		if (this.allowedUtmParameters && this.allowedUtmParameters.length > 0)
		{
			this._setUtmParameters(pageUrl);
		}

		try
		{
			// We assume we are the only element/advertiser inside DFP iframe
			var w = window;
			this.rootWindow = w.parent;
			do
			{
				var wf = w.frameElement;
				while (wf != null)
				{
					if (wf.parentNode.id.indexOf('div-gpt-ad') != -1)
					{
						this.frameInfo.isInsideGoogleFrame = true;
						break;
					}
					wf = wf.parentNode;
				}
				w = w.parent;
			} while (w != w.parent);
			this.rootWindow = w;
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] window element");
				console.log(w);
			}
		}
		catch(e)
		{
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] " + e.name + ": " + e.message);
			}
		}
		try
		{
			if (!this.frameInfo.isInsideGoogleFrame)
			{
				if (window.frameElement.id.indexOf('google_ads_iframe') != -1)
				{
					this.frameInfo.isInsideGoogleFrame = true;
					this.frameInfo.googleFrameId = window.frameElement.id;
					if (this.debug)
					{
						console.log("SEKDBG: [INFO] frame ID: " + this.frameInfo.googleFrameId);
						console.log("SEKDBG: [INFO] parent element");
						console.log(this.rootWindow);
					}
				}
			}
		}
		catch (e)
		{
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] - " + e.name + ": " + e.message);
			}
		}

		this.frameInfo.isBuildFrame = (this.needWrappingIframe && !this.frameInfo.isInsideGoogleFrame) || this.isAmpProject || (typeof this.config.playerDivSetup.elmtClassName !== 'undefined' || typeof this.config.playerDivSetup.elmtId !== 'undefined' || typeof this.config.playerDivSetup.querySelector !== 'undefined');
		/* Async exec requires isBuildFrame */
		// TODO:: should recognize and build iframe throgh JS also if document is ready/loaded
		this.frameInfo.isBuildFrameViaJs = (typeof this.config.playerDivSetup.elmtClassName !== 'undefined' || typeof this.config.playerDivSetup.elmtId !== 'undefined' || typeof this.config.playerDivSetup.querySelector !== 'undefined') || (this.frameInfo.isBuildFrame && this.srcElement && (this.srcElement.async || this.srcElement.defer)) || (this.frameInfo.isBuildFrame && (this.allowViewabilityCheck || this.allowUserScrollCheck));
		if (this.debug)
		{
			console.log("SEKDBG: [INFO] this.needWrappingIframe="+(this.needWrappingIframe ? 'yes' : 'no'));
			console.log("SEKDBG: [INFO] window.frameElement="+(window.frameElement ? 'ok' : 'n/a'));
			console.log("SEKDBG: [INFO] this.frameInfo.isBuildFrame="+(this.frameInfo.isBuildFrame ? 'yes' : 'no'));
			console.log("SEKDBG: [INFO] this.frameInfo.isInsideGoogleFrame="+(this.frameInfo.isInsideGoogleFrame ? 'yes' : 'no'));
			console.log("SEKDBG: [INFO] this.frameInfo.isBuildFrameViaJs="+(this.frameInfo.isBuildFrameViaJs ? 'yes' : 'no'));
			console.log("SEKDBG: [INFO] this.srcElement="+(this.srcElement ? 'ok' : 'n/a'));
			if (this.srcElement)
			{
				console.log("SEKDBG: [INFO] this.srcElement.async="+(this.srcElement.async ? 'yes' : 'no'));
				console.log("SEKDBG: [INFO] this.srcElement.defer="+(this.srcElement.defer ? 'yes' : 'no'));
			}
		}
		this._setBlockPlacementSettings(w.top);
		if (this.searchByMetaTagName)
		{
			var ref = this;
			Object.keys(this.searchByMetaTagName).forEach(function (idx)
			{
				if (typeof ref.searchByMetaTagName["tagAttribute"] === 'undefined')
				{
					// default meta tag attribute
					ref.searchByMetaTagName["tagAttribute"] = "name";
				}
				if (idx != "tagAttribute")
				{
					ref._setMetaTagHelperContent(w.top, ref.searchByMetaTagName["tagAttribute"], ref.searchByMetaTagName[idx], idx);
				}
			});
		}
		/* iPad client detection/set */
		if (this.isIpad)
		{
			this.url += '&pubDeviceType=Tablet';
			this.url += '&pubOs=iOS';
			this.url += '&pubOsVer=';
		}

		this._setSharedVideoParameter(pageUrl);
		this._setTargetingCustomParameter();
		this._setCustomPpid();
		this._setHelperParameters();
		this._setObserverChecker();
	}

	this._getTagContainerSizeInfo = function()
	{
		var containerInfo = [];
		var containerIframe = '';
		var iframeElement = null;
		containerInfo['urlParams'] = '&x=320&y=480';
		containerInfo['xLen'] = 320;
		containerInfo['yLen'] = 480;

		try
		{
			if (window.parent.document.querySelector('[id^=google_ads_iframe_dummy_sekindoParent]') != null)
			{
				containerIframe = window.parent.document.querySelector('[id^=google_ads_iframe_dummy_sekindoParent]').id;
				iframeElement = window.parent.document.getElementById(containerIframe);

				if (iframeElement.parentElement.parentElement.parentElement.clientWidth !== 0)
				{
					containerInfo['xLen'] = iframeElement.parentElement.parentElement.parentElement.clientWidth;
				}
				else if (iframeElement.parent.document.clientWidth !== 0)
				{
					containerInfo['xLen'] = iframeElement.parent.document.clientWidth;
				}

				if (iframeElement.parentElement.parentElement.parentElement.clientHeight !== 0)
				{
					containerInfo['yLen'] = iframeElement.parentElement.parentElement.parentElement.clientHeight;
				}
				else if (iframeElement.parent.document.clientHeight !== 0)
				{
					containerInfo['yLen'] = iframeElement.parent.document.clientHeight;
				}

				containerInfo['urlParams'] = '&x='+containerInfo['xLen']+'&y='+containerInfo['yLen'];
			}
			else if (this.frameInfo.isInsideGoogleFrame)
			{
				containerIframe = window.parent.document.querySelector('iframe[id^=google_ads_iframe]').id;
				iframeElement = window.parent.document.getElementById(containerIframe);
				containerInfo['urlParams'] = '&x='+iframeElement.clientWidth+'&y='+iframeElement.clientHeight;
			}
		}
		catch (e)
		{
			if (this.debug)
			{
				console.log("SEKDBG: Exception get DFP or Dummy iframe element");
			}
		}

		return containerInfo;
	}

	this.process = function()
	{
		if (!this.isRun)
			return;

		if (this.config.unit.width == 0)
			return;
		if ((!this.observedElementInfo.isViewable && this.allowViewabilityCheck) || (!this.observedElementInfo.hasScrolled && this.allowUserScrollCheck))
			return;
		if (this.ci.browser == 'ie')
			return;

		var ref = this;
		var url = this.url;

		if (this.frameInfo.isBuildFrame)
		{
			var constructed = false;
			var uniqueID = 'sekindoParent'+Math.round(Math.random()*1000);
			window['construct'+uniqueID] = function (iframe)
			{
				if (constructed) return;
				constructed = true;
				if (iframe.contentWindow || iframe.contentDocument)
				{
					var iFramewindow = iframe.contentWindow || iframe.contentDocument.defaultView;
					var iFrameDoc = iFramewindow.document || iframe.contentDocument;

					if (ref.isResponsiveBanner)
					{
						var tagContainerSizeInfo = ref._getTagContainerSizeInfo();
						if (tagContainerSizeInfo['urlParams'] != '')
						{
							url += tagContainerSizeInfo['urlParams'];
							ref.config.unit.width = tagContainerSizeInfo['xLen'];
							ref.config.unit.height = tagContainerSizeInfo['yLen'];
						}
					}


					if (ref._getPlacementType() === 'banner') // use old method. (document.write)
					{
						ref._docWrite(iFrameDoc, uniqueID, url);
					}
					else
					{
						//Make dom ready.
						iFrameDoc.open();
						iFrameDoc.close();
						// If appendChild fails - go to document.write
						try
						{
							var script1 = iFrameDoc.createElement('script');
							script1.text = "window.baseIframeName = 'google_ads_iframe_dummy_" + uniqueID +"'"
							iFrameDoc.head.appendChild(script1);

							var base = document.createElement('base');
							base.href = 'https://amli.sekindo.com/';
							iFrameDoc.head.appendChild(base);

							var script = document.createElement('script');
							script.src = url;
							iFrameDoc.head.appendChild(script);
						}
						catch(e)
						{
							ref._docWrite(iFrameDoc, uniqueID, url);
						}
					}
				}
			}

			if (this.frameInfo.isBuildFrameViaJs)
			{
				var iframe = document.createElement('iframe');
				var div0 = document.createElement('div');
				var div1 = document.createElement('div');
				div1.id = 'primisPlayerContainerDiv';

				iframe.marginWidth = '0';
				iframe.marginHeight = '0';
				iframe.hspace = '0';
				iframe.vspace = '0';
				if (this.isAPI) iframe.height = '0';
				iframe.frameBorder = '0';
				iframe.scrolling = 'no';
				iframe.title = 'Primis Frame';
				iframe.id = 'google_ads_iframe_dummy_'+uniqueID;
				if (this._getPlacementType() == 'banner')
				{
					iframe.style.width = ref.config.unit.width + "px";
					iframe.style.height = ref.config.unit.height + "px";
				}
				else
				{
					iframe.style.width = '1px';
					iframe.style.height = '1px';
				}

				if (typeof this.config.playerDivSetup !== 'undefined' && typeof this.config.playerDivSetup.insertPosition !== 'undefined')
				{
					if (this.config.playerDivSetup.insertPosition == "inside")
					{
						this.srcElement.appendChild(div0);
					}
					else if (this.config.playerDivSetup.insertPosition == "after")
					{
						this.srcElement.parentNode.insertBefore(div0, this.srcElement.nextElementSibling);
					}
					else if (this.config.playerDivSetup.insertPosition == "before")
					{
						this.srcElement.parentNode.insertBefore(div0, this.srcElement);
					}
				}
				else
				{
					if (this.srcElement)
					{
						this.srcElement.parentNode.insertBefore(div0, this.srcElement);
					}
				}
				div0.appendChild(div1);
				div1.appendChild(iframe);
				window['construct'+uniqueID](iframe);
			}
			else
			{
				if (this._getPlacementType() === 'banner') // use old method. (document.write)
				{
					var code = '<div><div><iframe width="' +  this.config.unit.width + '" height="' +  this.config.unit.height + '" marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" scrolling="no" onload="construct'+uniqueID+'(this)" title ="Primis Frame" id ="google_ads_iframe_dummy_'+uniqueID+'"></iframe></div></div>';
					document.write(code);
				}
				else
				{
					var code = '<div><iframe style="width:1px; height:1px;"  marginwidth="0" marginheight="0" hspace="0" vspace="0" frameborder="0" scrolling="no" onload="construct'+uniqueID+'(this)" title ="Primis Frame" id ="google_ads_iframe_dummy_'+uniqueID+'"></iframe></div>';
					try
					{
						var div = document.createElement('div');
						div.innerHTML = code;
						var currScript = this.getCurrScript()
						currScript.parentNode.insertBefore(div, currScript);
					}
					catch(e)
					{
						code = '<div>'+code+'</div>';
						document.write(code);
					}
				}
			}
		}
		else
		{
			if (ref.isResponsiveBanner)
			{
				var tagContainerSizeInfo = ref._getTagContainerSizeInfo();
				if (tagContainerSizeInfo['urlParams'] != '')
				{
					url += tagContainerSizeInfo['urlParams'];
				}
			}
			if (this._getPlacementType() === 'banner') // use old method. (document.write)
			{
				document.write(unescape("%3Cscript src='") + url + unescape("' type='text/javascript'%3E%3C/script%3E"));
			}
			else
			{
				try
				{
					var script = document.createElement('script');
					script.src = url;
					document.head.appendChild(script);
				}
				catch(e)
				{
					document.write(unescape("%3Cscript src='") + url + unescape("' type='text/javascript'%3E%3C/script%3E"));
				}
			}

		}
	}

	this.getCurrScript = function(element, checkLimit)
	{
		return document.currentScript || document.scripts[document.scripts.length-1] ;
	};

	this._docWrite = function(iFrameDoc, uniqueID, url)
	{
		iFrameDoc.open();
		iFrameDoc.write(unescape("%3Cscript%3Evar baseIframeName = 'google_ads_iframe_dummy_") + uniqueID + unescape("'%3C/script%3E") + "<base href='https://amli.sekindo.com/'>" + unescape("%3Cscript src='") + url + unescape("' type='text/javascript'%3E%3C/script%3E"));
		iFrameDoc.close();
	}

	this._getPlacementType = function()
	{
		if (this.placementType && this.placementType !== 'slBanner' && this.placementType !== 'outstream' && this.placementType !== 'vast')
		{
			return 'banner';
		}
		return 'video';
	}

	this._getPlayerMainElement = function(orgWin)
	{
		var sourceElmt;
		if (window.frameElement !== null)
		{
			var pdmt, contElmt, gfrm=window.frameElement.id;
			pdmt 	 = window.parent.document.getElementById(gfrm);
			contElmt = pdmt.parentNode.parentNode.parentNode; // element before DFP div
			orgWin   = contElmt.ownerDocument.defaultView || contElmt.ownerDocument.parentWindow;
		}
		try
		{
			if (this.debug)
			{
				console.log("SEKDBG: mainElement owner");
				console.log(orgWin);
			}
			sourceElmt = this._getSourceElementByType(orgWin.top.document, this.config.playerDivSetup);
		}
		catch (e)
		{
			if (this.debug)
			{
				console.log("SEKDBG: Exception get mainElement");
			}
			sourceElmt = this._getSourceElementByType(orgWin.document, this.config.playerDivSetup);
		}
		if (this.debug)
		{
			console.log("SEKDBG: playerDivSetup mainElement");
			console.log(sourceElmt);
		}

		if (sourceElmt && typeof sourceElmt !== 'undefined' && typeof this.config.playerDivSetup.childElement !== 'undefined')
		{
			sourceElmt = this._getSourceElementByType(sourceElmt, this.config.playerDivSetup.childElement);
			if (this.debug)
			{
				console.log("SEKDBG: playerDivSetup childElement");
				console.log(sourceElmt);
			}
		}

		if (typeof this.config.playerDivSetup.childElementNumber !== 'undefined' && typeof sourceElmt !== 'undefined')
		{
			if (typeof this.config.playerDivSetup.childElementTagName !== 'undefined')
			{
				sourceElmt = sourceElmt.getElementsByTagName(this.config.playerDivSetup.childElementTagName)[this.config.playerDivSetup.childElementNumber - 1];
			}
			else
			{
				for (var i = 0; i < sourceElmt.childElementCount; i++)
				{
					if (i == this.config.playerDivSetup.childElementNumber - 1)
					{
						sourceElmt = sourceElmt.children[i];
					}
				}
			}
		}
		return sourceElmt;
	}

	this._getSourceElementByType = function(orgWin, playerDivSetup)
	{
		var sourceElmt = null;
		if (typeof playerDivSetup.elmtClassName !== 'undefined')
		{
			if (typeof playerDivSetup.mainElementNumber !== 'undefined')
			{
				sourceElmt = orgWin.getElementsByClassName(playerDivSetup.elmtClassName)[playerDivSetup.mainElementNumber-1];
			}
			else
			{
				sourceElmt = orgWin.getElementsByClassName(playerDivSetup.elmtClassName)[0];
			}
		}
		else if (typeof playerDivSetup.elmtId !== 'undefined')
		{
			sourceElmt = orgWin.getElementById(playerDivSetup.elmtId);
		}
		else if (typeof playerDivSetup.querySelector !== 'undefined')
		{
			sourceElmt = orgWin.querySelector(this.config.playerDivSetup.querySelector);
		}
		return sourceElmt;
	}

	this._setUtmParameters = function(pageUrl)
	{
		var d_utm = '', ref = this;
		this.url = this.url.replace('&subId=[SUBID_ENCODED]','');
		if (pageUrl != '')
		{
			var parts = pageUrl.replace(/[?&#]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
				for (var i = 0; i < ref.allowedUtmParameters.length; i++)
				{
					if (key.toLowerCase() == ref.allowedUtmParameters[i])
					{
						d_utm += '-'+value;
					}
				}
			});
			d_utm = d_utm.substring(1);
		}
		if (d_utm == '')
		{
			d_utm = 'default';
		}
		this.url += '&subId=' + d_utm;
	}

	this._setBlockPlacementSettings = function(orgWin) {
		if (this.placementId != -1) {
			try {
				var addBlockParameter = false;

				// Block same placement
				if (typeof orgWin.sekindoDisplayedPlacement === 'undefined') {
					orgWin.sekindoDisplayedPlacement = this.placementId;
					if (this.debug) {
						console.log("SEKDBG: Displayed placement: "+this.placementId);
					}
				}
				else if (!this.allowDisplaySamePlacementAgain && orgWin.sekindoDisplayedPlacement == this.placementId) {
					addBlockParameter = true;
					if (this.debug) {
						console.log("SEKDBG: Allow display the same placement: "+this.allowDisplaySamePlacementAgain);
						console.log("SEKDBG: Placement ID verification: "+this.placementId);
						console.log("SEKDBG: Displayed placement on page: "+orgWin.sekindoDisplayedPlacement);
					}
				}
				// Block other placements
				if (!addBlockParameter) {
					if (this.blockOtherPlacements && typeof orgWin.sekindoBlockOtherPlacements === 'undefined') {
						orgWin.sekindoBlockOtherPlacements = true;
						if (this.debug) {
							console.log("SEKDBG: Displayed placement: "+this.placementId);
						}
					}
					else if (orgWin.sekindoBlockOtherPlacements) {
						addBlockParameter = true;
						if (this.debug) {
							console.log("SEKDBG: Block other placements: "+this.blockOtherPlacements);
						}
					}
				}

				if (addBlockParameter) { // add block parameter to second call
					this.url += '&blockPlacement=1';
				}
			}
			catch (e) {}
		}
	};

	this._getMetaTagContent = function(orgWin, property, searchPropertyValue) {
		var tagContent = null;
		var metaTags = [];
		try {
			metaTags = orgWin.document.getElementsByTagName("meta");
		} catch (e) {}
		for (var idx = 0; idx < metaTags.length; idx++) {
			if (typeof metaTags[idx].getAttribute(property) !== 'undefined' && metaTags[idx].getAttribute(property) === searchPropertyValue) {
				if (typeof metaTags[idx].getAttribute("content") !== 'undefined') {
					tagContent = metaTags[idx].getAttribute("content");
					break;
				}
			}
		}
		return tagContent;
	};

	this._setMetaTagHelperContent = function(w, property, searchProperty, contentName)
	{
		var metaTagContent = this._getMetaTagContent(w, property, searchProperty);
		if (metaTagContent)
		{
			this.helperParameters[contentName] = metaTagContent;
		}
	};

	this._setHelperParameters = function()
	{
		if (Object.keys(this.helperParameters).length > 0)
		{
			// convert helper parameters object to JSON base64 encoded string
			var jsonFormattedHelperParameters = encodeURIComponent(JSON.stringify(this.helperParameters));
			this.url += '&'+this.VIDEO_HELPER_PARAM_NAME+'='+window.btoa(jsonFormattedHelperParameters);
		}
	};

	this._setSharedVideoParameter = function(pageUrl)
	{
		var s_content = '', ref = this;
		if (pageUrl != '')
		{
			var parts = pageUrl.replace(/[?&#]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
				if (key.toLowerCase() == ref.VIDEO_SHARED_PARAM_NAME)
				{
					s_content = value;
				}
			});
		}
		if (s_content !== '' && s_content.indexOf("embed") != -1)
		{
			this.helperParameters[ref.VIDEO_SHARED_PARAM_NAME] = s_content;
		}
	};

	this._setTargetingCustomParameter = function()
	{
		var custom_parameters = {};
		if (this.customTargetingProperties && Object.keys(this.customTargetingProperties).length > 0)
		{
			if (typeof this.customTargetingProperties["objectType"] !== 'undefined' && typeof this.customTargetingProperties["objectFunction"] !== 'undefined')
			{
				try
				{
					custom_parameters = eval(this.customTargetingProperties["objectType"]+"."+this.customTargetingProperties["objectFunction"]);
					if (Object.keys(custom_parameters).length > 0)
					{
						if (typeof custom_parameters == 'string')
						{
							var tmp = {};
							tmp[this.customTargetingProperties["objectFunction"]] = [custom_parameters];
							custom_parameters = tmp;
						}
						this.helperParameters[this.VIDEO_GAM_TARGETING_PARAM_NAME] = custom_parameters;
					}
				}
				catch (e) {}
			}
		}
	};

	this._setCustomPpid = function() {
		if (this.customPpidMapping && Object.keys(this.customPpidMapping).length > 0) {
			var customPpidParameter = {};
			if (typeof this.customPpidMapping["objectType"] !== 'undefined' && typeof this.customPpidMapping["objectFunction"] !== 'undefined') {
				try {
					customPpidParameter = eval(this.customPpidMapping["objectType"]+"."+this.customPpidMapping["objectFunction"]);
					if (Object.keys(customPpidParameter).length > 0) {
						this.helperParameters['ppid'] = customPpidParameter;
					}
				}
				catch (e) {}
			}
		}
	};

	this._getViewportSize = function(w)
	{
		if (w.innerWidth != null)
			return {w:w.innerWidth, h:w.innerHeight};

		var d = w.document;
		if (document.compatMode == "CSS1Compat")
			return {w: d.documentElement.clientWidth, h: d.documentElement.clientHeight};

		return {w: d.body.clientWidth, h: d.body.clientWidth};
	};

	this._setProb = function()
	{
		try {
			viewPortSize = this._getViewportSize(window.top);
		}
		catch (e) {
			viewPortSize = this._getViewportSize(window);
		}

		this.prob.window = {};
		this.prob.window.width = viewPortSize.w;
		this.prob.window.height = viewPortSize.h;
		this.prob.geo = this.geo;
		this.prob.ci = this.ci;
		this.prob.ci.geo = this.geo;
		this.prob.dmaCode = this.dmaCode;
		this._setIpadConfig();
	}

	this._setIpadConfig = function()
	{
		try
		{
			if (this.url.indexOf('pubDeviceType') == -1 && navigator.platform === 'MacIntel' && navigator.maxTouchPoints >= 5 && typeof navigator.standalone !== "undefined")
			{
				this.isIpad = true;
				this.prob.ci.deviceType = "tablet";
			}
		}
		catch (e){}
	}

	this._setConfig = function()
	{
		this.config = {};
		this.config.unit = {};
		this.config.unit.width =  this.x;
		this.config.unit.height = this.y;
		this.config.floatConfig = this.floatConfig;
		this.config.dynamicSetup = this.dynamicSetup;
		this.config.videoType 	= this.videoType;
		this.config.playerDivSetup = {};
		this.config.url = this.url;
	}

	this._setReplaceUrlParam = function(paramName, newValue)
	{
		var regex = new RegExp("&"+paramName+"=[^&]+", "gm");
		this.url = this.url.replace(regex, '');
		regex = new RegExp("[?]"+paramName+"=[^&]+", "gm");
		this.url = this.url.replace(regex, '?');
		this.url += "&" + paramName + "=" +  newValue;
	}

	this._setObserverChecker = function()
	{
		try
		{
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] browser="+this.ci.browser+", application="+this.isApp);
			}
			if (!this.isApp && !this.isAmpProject && !this.isAmpIframe && (this.ci.browser == 'chrome' || this.ci.browser == 'edgeChromium')
				&& (this.allowViewabilityCheck || this.allowUserScrollCheck))
			{
				this._setObserverElement();
				this._observerChecker();
			}
			else
			{
				this.allowViewabilityCheck = false;
				this.allowUserScrollCheck  = false;
			}
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] viewable="+this.allowViewabilityCheck+", scroll="+this.allowUserScrollCheck);
			}
		}
		catch (e)
		{
			this.allowViewabilityCheck = false;
			this.allowUserScrollCheck  = false;
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] "+e.name + ": " + e.message);
			}
		}
	}

	this._observerChecker = function()
	{
		var observerOptions, currentScrollVerticalPosition=1, ref=this;
		if (ref.observerElement)
		{
			observerOptions = {
				root: null,
				rootMargin: '0px',
				threshold: [config.playerInViewPrc, config.adInViewPrc]
			}
			this.elementChecker = new ref.rootWindow.IntersectionObserver(intersectionCallback, observerOptions);
			this.elementChecker.observe(ref.observerElement);
		}

		function intersectionCallback(entries, observer)
		{
			var intersectionEntry = entries[entries.length - 1];
			var currentViewedPercentage = intersectionEntry.intersectionRatio;
			ref.observedElementInfo.isViewable  = (currentViewedPercentage > ref.observedElementInfo.viewabilityThreshold);
			ref.observedElementInfo.hasScrolled = (currentViewedPercentage != currentScrollVerticalPosition);
			if (ref.debug)
			{
				console.log("SEKDBG: [INFO] percentage="+currentViewedPercentage+", in viewable="+ref.observedElementInfo.isViewable+", after scroll="+ref.observedElementInfo.hasScrolled);
			}
			currentScrollVerticalPosition = currentViewedPercentage;
			if ((ref.observedElementInfo.isViewable && ref.allowViewabilityCheck) || (ref.observedElementInfo.hasScrolled && ref.allowUserScrollCheck))
			{
				ref.elementChecker.unobserve(ref.observerElement);
				ref.process();
			}
		}
	}

	this._setObserverElement = function()
	{
		this.observerElement = null;
		if (this.allowUserScrollCheck)
		{
			var widgetElmt, mElmt, elmtId='extendedSection';
			mElmt = this.rootWindow.document.createElement('widget-markup');
			mElmt.setAttribute('id', elmtId);
			mElmt.setAttribute('style', 'width:0px; height:0px;');
			if (this.debug)
			{
				console.log("SEKDBG: [INFO] firstChild");
				console.log(this.rootWindow.document.body.firstChild);
			}
			this.rootWindow.document.body.insertBefore(mElmt, this.rootWindow.document.body.firstChild);
			widgetElmt = this.rootWindow.document.getElementById(elmtId);
			if (widgetElmt)
			{
				this.observerElement = widgetElmt;
			}
		}
		else if (typeof this.config.playerDivSetup.elmtClassName !== 'undefined' || typeof this.config.playerDivSetup.elmtId !== 'undefined' || typeof this.config.playerDivSetup.querySelector !== 'undefined')
		{
			this.observerElement = this.srcElement;
		}
		else if (this.srcElement && this.srcElement.parentNode)
		{
			if (this.frameInfo.isInsideGoogleFrame && typeof this.frameInfo.googleFrameId !== 'undefined')
			{
				this.observerElement = this.rootWindow.document.getElementById(this.frameInfo.googleFrameId);
			}
			else
			{
				this.observerElement = this.srcElement.parentNode;
			}
		}
		if (this.debug)
		{
			console.log("SEKDBG: [INFO] observer element");
			console.log(this.observerElement);
		}
		if (!this.observerElement)
		{
			this.allowViewabilityCheck = false;
			this.allowUserScrollCheck  = false;
		}
	}

	this.playerApiActions = function(){
		function PrimisApiConfig (){
			this.setConfig = function (apiConfig){
				this.apiPlayerConfig = apiConfig;
			}
		}

		window.top.PrimisApiConfig = new PrimisApiConfig();
		window.top.dispatchEvent(new CustomEvent('readyConfigPlayerApi', {detail: window.top.PrimisApiConfig}));
	}

	this._setProb();
	this._setConfig();

	var dynamicConfig = new SekindoClientDynamicConfig(this.config, this.prob);
	dynamicConfig.run();
	// dynamicConfig can change this url
	this.url = this.config.url;

	this.getScriptElement();
	this.setInfo();
	if (this.isPlayerApiActions && !this.isAmpProject && !this.isAmpIframe && !this.isApp) this.playerApiActions();
};

				var urlDetObj = new SekindoClientDetections_URL({
					url: 'https://live.primis.tech/live/liveView.php?s=116190&schain=1.0,1!newormedia.com,8cfc671c-fc7c-413c-9138-466456c72e50,1&cbuster=984129491&cbuster=1700327731',
					origQString: 's=116190&schain=1.0,1!newormedia.com,8cfc671c-fc7c-413c-9138-466456c72e50,1&cbuster=984129491',
					placementId: 116190,
					x: 300,
					y: 250,
					videoType: 'flow',
					needWrappingIframe: 1,
					isAmpProject: 0,
					isAmpIframe: 0,
					floatConfig: {
						width: '', height: '',
						direction: '', verticalOffset: '',
						horizontalOffset: '', isCloseBtn: '',
						flowMode: '', closeBtnPos: ''
					},
					isAPI: false,
					debug: 0,
					ci: {"extra":{"schemaVer":"11","os":"Ubuntu","osVersion":"","osVersionMajor":"0","osVersionMinor":"0","deviceManufacturer":"","deviceModel":"","deviceCodeName":"","deviceType":"desktop","browser":"Firefox","browserType":"browser","browserVersion":"113.0","browserVersionMajor":"113","browserVersionMinor":"0","chromeVersion":""},"browser":"firefox","os":"linux","osVer":"","deviceType":"desktop","languages":{"0":"fi","2":"en"},"googlePSI":false},
					isFpCookie: 1,
					geo: 'FI',
					dmaCode: 0,
					dynamicSetup: [],
					uuid: '6558f1330895d',
					isResponsiveBanner: false,
					hideOnMissingMainElement: false,
					allowedUtmParameters: [],
					allowDisplaySamePlacementAgain: true,
					blockOtherPlacements: false,
					searchMetaTagNames: false,
					customTargetingProperties: false,
					customPpidMapping: false,
					sharedVideoParameterName: 'primis_content',
					gamTargetingVideoParameterName: 'primis_custom_target',
                    placementType: 'slBanner',
					isApp: false,
					playerInViewPrc: 0.01,
					adInViewPrc: 0.5,
					allowViewabilityCheck: false,
					allowUserScrollCheck: false,
					isRun: 1,
					isPlayerApiActions: false				});
				urlDetObj.process();
			})();
