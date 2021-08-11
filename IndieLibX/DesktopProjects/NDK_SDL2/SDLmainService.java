package org.libsdl.app;

import android.app.IntentService;
import android.content.Intent;
import android.os.Bundle;
import android.os.ResultReceiver;
import android.text.TextUtils;
import android.util.Log;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import android.util.Log;

public class SDLmainService extends IntentService {

    public static final int STATUS_RUNNING = 0;
    public static final int STATUS_FINISHED = 1;
    public static final int STATUS_ERROR = 2;

    private static final String TAG = "SDLmainService";

    public SDLmainService() {
        super(SDLmainService.class.getName());
    }

    @Override
    protected void onHandleIntent(Intent intent) {

        Log.d("SDLmainService", "Service Started!");
	
	SDLActivity.nativeSDLmain();

        Log.d("SDLmainService", "Service Stopping!");
        this.stopSelf();
    }
}

