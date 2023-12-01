!function (t, u) {
    const COOKIE_TTL = 57600; // (16 * 60 * 60);
    const CURRENT_SECONDS = Math.round(new Date() / 1000);
    
    const GDPR_APPLIES = true;
    
    const NEED_GDPR_FLAGS = ['apn', 'ttd', 'rub', 'smart', 'son', 'index'];
    const NEED_HADRON_MATCH = ['index'];
    const ID_MATCH_VENDORS = {
        "apn": 32, "ttd": 21, "adx": 755, "ado": 565, "pub": 76, "son": 104, "goo": 561, "rub": 52, "bees": 12,
        "impr": 253, "smart": 45, "ppnt": 81, "taboola": 42, "unruly": 36, "openx": 69, "ip": 561,
        "tapad": 89, "index": 10
    }

    function getCookie(d, key) {
        let fkey = "_au_" + key + "=";
        let decodedCookie = decodeURIComponent(d.cookie);
        let ca = decodedCookie.split(';');
        for (let i = 0; i < ca.length; i++) {
            let c = ca[i];
            while (c.charAt(0) == ' ') {
                c = c.substring(1);
            }
            if (c.indexOf(fkey) == 0) {
                return c.substring(fkey.length, c.length);
            }
        }
        return null;
    }

    function setCookie(d, key, value, maxAge = 31536000) {
        d.cookie = "_au_" + key + "=" + encodeURIComponent(value) + "; domain=" + extractDomain(d.location.hostname) + "; secure; samesite=none; path=/; max-age=" + maxAge;
    }

    function imgSrcToElement(src, d) {
        var img = d.createElement('img');
        img.height = 1;
        img.width = 1;
        img.style.display = 'none';
        img.src = src;
        img.alt = '';
        return img;
    }

    function scriptSrcToElement(w, d, t, u) {
        var a = d.createElement(t);
        a.async = true;
        a.crossorigin = 'anonymous';
        a.src = u;
        var s = d.getElementsByTagName(t)[0];
        s.parentNode.insertBefore(a, s);
    }

    // naive way to extract domain name (example.com) from full hostname (my.sub.example.com)
    const SIMPLE_DOMAIN_MATCH_REGEX = /[a-z0-9][a-z0-9-]*\.[a-z]+$/i;
    // this next one attempts to account for some ccSLDs, e.g. extracting oxford.ac.uk from www.oxford.ac.uk
    const DOMAIN_MATCH_REGEX = /[a-z0-9][a-z0-9-]+\.[a-z.]{2,6}$/i;

    function extractDomain(hostname) {
        let domain_regex = DOMAIN_MATCH_REGEX;
        let parts = hostname.split('.');
        let tld = parts[parts.length - 1];
        if (tld.length > 4 || tld === 'com' || tld === 'org' || tld === 'net') {
            domain_regex = SIMPLE_DOMAIN_MATCH_REGEX;
        }
        let matches = hostname.match(domain_regex);
        return matches ? matches[0] : hostname;
    };

    function randomString(length) {
        let chars = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXTZ'.split('');

        if (!length) {
            length = Math.floor(Math.random() * chars.length);
        }

        let str = '';
        for (let i = 0; i < length; i++) {
            str += chars[Math.floor(Math.random() * chars.length)];
        }
        return str;
    }

    function buildAudigent1d() {
        const secondsSinceEpoch = Math.round(Date.now() / 1000);
        return "AU1D-0100-" + String(secondsSinceEpoch).padStart(12, '0') + "-" + randomString(8) + "-" + randomString(4);
    }

    function getAudigent1d(w, d) {
        let au1d = getCookie(d, "1d");
        if (au1d == null) {
            au1d = buildAudigent1d();
        }
        return au1d;
    }

    function setupHadron(w, d, au1d) {
        if ((w.hadron || { loaded: false }).loaded === true) {
            // get HadronID
            const hadronId = w.hadron.id;
            // cookie sync
            const imgSrc = 'https://ids.ad.gt/api/v1/halo_match?id=' + au1d + '&halo_id=' + hadronId;
            const img = imgSrcToElement(imgSrc, d);
            d.body.appendChild(img);
            return hadronId;
        } else {
            // load hadron script
            scriptSrcToElement(w, d, 'script', 'https://cdn.hadronid.net/hadron.js?partner_id=597&sync=1&url=' + encodeURIComponent(w.location.href));
            return null;
        }
    }

    function setupCookieSync(w, d, au1d, hadronId, tcdata) {
        const pixels = [["apn", "https://secure.adnxs.com/getuid?https://ids.ad.gt/api/v1/match?id=[AU1D]&adnxs_id=$UID"], ["ttd", "https://match.adsrvr.org/track/cmf/generic?ttd_pid=8gkxb6n&ttd_tpi=1&ttd_puid=[AU1D]"], ["pub", "https://image2.pubmatic.com/AdServer/UCookieSetPug?rd=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fpbm_match%3Fpbm%3D%23PM_USER_ID%26id%3D[AU1D]"], ["rub", "https://token.rubiconproject.com/token?pid=50242&puid=[AU1D]"], ["tapad", "https://pixel.tapad.com/idsync/ex/receive?partner_id=3185&partner_device_id=[AU1D]&partner_url=https://ids.ad.gt%2Fapi%2Fv1%2Ftapad_match%3Fid%3D[AU1D]%26tapad_id%3D%24%7BTA_DEVICE_ID%7D"], ["adx", "https://cm.g.doubleclick.net/pixel?google_nid=audigent_w_appnexus_3985&google_cm&google_sc&google_ula=450542624&id=[AU1D]"], ["goo", "https://ids.ad.gt/api/v1/g_hosted?id=[AU1D]"], ["bees", "https://match.prod.bidr.io/cookie-sync/audigent?buyer_user_id=[AU1D]"], ["index", "https://ssum-sec.casalemedia.com/ium?sourceid=15&uid=[HID]"], ["openx", "https://u.openx.net/w/1.0/cm?id=998eaf06-9905-4eae-9e26-9fac75960c53&r=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fopenx%3Fopenx_id%3D%7BOPENX_ID%7D%26id%3D[AU1D]%26auid%3D[AU1D]"], ["impr", "https://ad.360yield.com/ux?&publisher_dmp_id=15&r=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fimpr_match%3Fid%3D[AU1D]%26impr_uid%3D%7BPUB_USER_ID%7D"], ["colossus", "https://sync.colossusssp.com/ebfa23da174faa55634171c5e49d0152.gif?puid=[AU1D]&redir=http%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fcolossus%3Fcls_id%3D%5BUID%5D%26id%3D[AU1D]"], ["unruly", "https://sync.1rx.io/usersync/audigent/0?dspret=1&redir=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Funruly%3Fid%3D[AU1D]%26unruly_id%3D%5BRX_UUID%5D"], ["ip", "https://ids.ad.gt/api/v1/ip_match?id=[AU1D]"], ["amo", "https://d.turn.com/r/dd/id/L2NzaWQvMS9jaWQvMTc0ODI0MTY1OC90LzA/url/https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Famo_match%3Fturn_id%3D%24!{TURN_UUID}%26id%3D[AU1D]"], ["smart", "https://sync.smartadserver.com/getuid?url=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fsmart_match%3Fid%3D[AU1D]%26sas_uid%3D%5bsas_uid%5d"], ["ado", "https://dpm.demdex.net/ibs:dpid=348447&dpuuid=[AU1D]&redir=https%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Fadb_match%3Fadb%3D%24%7BDD_UUID%7D%26id%3D[AU1D]"], ["ppnt", "https://bh.contextweb.com/bh/rtset?pid=562316&ev=1&rurl=https://ids.ad.gt/api/v1/ppnt_match?uid=%%VGUID%%&id=[AU1D]"], ["taboola", "https://trc.taboola.com/sg/audigent/1/cm?redirect=http%3A%2F%2Fids.ad.gt%2Fapi%2Fv1%2Ftaboola%3Fpartner_uid%3D%3CTUID%3E%3Fid%3D[AU1D]"], ["son", "https://sync.go.sonobi.com/us?https://ids.ad.gt/api/v1/son_match?id=[AU1D]&uid=[UID]"]];
        let pixelCount = 0;
        const lastSeenPixelsEncoded = getCookie(d, "last_seen_pixels");
        const lastSeenPixels = lastSeenPixelsEncoded !== null ? JSON.parse(atob(lastSeenPixelsEncoded)) : {};
        for (const pixelInfo of pixels) {
            if (pixelCount > 9)
                break;

            const pixelType = pixelInfo[0];
            setCookie(d, "last_seen_" + pixelType, "", 0); // Cleanup legacy

            let pixelHref = "";
            if (NEED_HADRON_MATCH.includes(pixelType)) {
                // I depend on the Hadron script here, so I am trying to do my best with what I have got
                if (hadronId == null) {
                    continue;
                }
                pixelHref = pixelInfo[1].replace(/\[HID\]/g, hadronId);
            } else {
                // replacement of the audigent ID done in the browser
                pixelHref = pixelInfo[1].replace(/\[AU1D\]/g, au1d);
            }

            if ((tcdata || {})['gdprApplies'] === true) {
                const vendorId = ID_MATCH_VENDORS[pixelType];
                if (vendorId === undefined) {
                    continue;
                }
                const vendorConsents = tcdata['vendor'].consents;
                if (vendorConsents[vendorId] !== true) {
                    continue;
                }
            }

            const lastSeenMillis = parseInt(lastSeenPixels[pixelType]) || 0;
            if (lastSeenMillis + COOKIE_TTL < CURRENT_SECONDS) {
                if (NEED_GDPR_FLAGS.includes(pixelType)) {
                    if ((tcdata || {})['gdprApplies'] === true) {
                        pixelHref += ('&gdpr=1&gdpr_consent=' + (tcdata['tcString'] || ''));
                    } else {
                        pixelHref += ('&gdpr=0');
                    }
                }

                const img = imgSrcToElement(pixelHref, d);
                d.body.appendChild(img);

                if (pixelType !== 'ip') {
                    lastSeenPixels[pixelType] = CURRENT_SECONDS
                }
                pixelCount += 1;
            }
        }
        setCookie(d, "last_seen_pixels", btoa(JSON.stringify(lastSeenPixels)));
    }

    function isPermittedToProcessData(tcdata) {
        let permitted = false;
        if ((tcdata || {})['gdprApplies'] === true) {
            const consents = tcdata['purpose']['consents']
            const permittedConsents = Object.keys(consents).filter(idx => consents[idx] === true);
            if (tcdata['vendor'].consents[561] &&
                ['1', '2', '3', '4', '9'].every(v => permittedConsents.includes(v))) {
                permitted = true;
            }
        }
        return permitted;
    }

    function processData(w, d, t, u, tcdata) {
        w.auvars = w.auvars || {};
        w.auvars['597'] = w.auvars['597'] || {};

        if (w.auvars['597'].a_loaded === true) {
            console.warn("[Audigent] script has already been initialised. Skip further execution");
            return;
        }

        const au1d = getAudigent1d(w, d);
        const hadronId = setupHadron(w, d, au1d);

        setCookie(d, "1d", au1d);

        // global audigent ID for all partners
        w.auvars.a = au1d;

        w.auvars['597'].a_loaded = true;
        w.auvars['597'].p = '597';
        w.auvars['597'].a = au1d;
        w.auvars['597'].ty = 'passive';
        w.auvars['597'].tcdata = tcdata;

        

        

                

        setupCookieSync(w, d, au1d, hadronId, tcdata);
        
        var a = d.createElement(t);
        a.defer = true;
        a.src = u;
        var s = d.getElementsByTagName(t)[0];
        s.parentNode.insertBefore(a, s);
    }

    function onConsent(w, d, t, u, callback) {
        const tcdataCallback = (tcdata, success) => {
            if (success && tcdata.eventStatus === 'tcloaded') {
                // remove the ourself to not get called more than once
                __tcfapi('removeEventListener', 2, (success) => {
                    if (success) {
                        // oh good...
                    }
                }, tcdataCallback);

                if (isPermittedToProcessData(tcdata)) {
                    callback(w, d, t, u, tcdata);
                }
            }

            if (success && tcdata.eventStatus === 'useractioncomplete') {
                if (isPermittedToProcessData(tcdata)) {
                    callback(w, d, t, u, tcdata);
                }
            }
        }

        const gppCallback = (evt) => {
            console.warn('[Audigent] implementation for __gpp framework to be completed');
        }

        if (GDPR_APPLIES === false) {
            processData(w, d, t, u, { "gdprApplies": false });
        } else if (typeof w['__tcfapi'] === 'function') {
            __tcfapi('addEventListener', 2, tcdataCallback);
        } else if (typeof w['__gpp'] === 'function') {
            __gpp('addEventListener', gppCallback);
        } else {
            console.warn('[Audigent] cannot find a required consent framework, halt execution');
        }
    }

    const w = window;

    onConsent(w, w.document, t, u, processData);

}('script', 'https://p.ad.gt/api/v1/p/597');