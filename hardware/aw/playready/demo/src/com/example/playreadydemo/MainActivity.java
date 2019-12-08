
package com.example.playreadydemo;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.os.Bundle;
import android.os.Parcel;
import android.app.Activity;
import android.util.Base64;
import android.util.Log;
import android.view.Menu;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.Toast;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.URL;

public class MainActivity extends Activity implements View.OnClickListener {
    
    private static final String TAG = "playready";
    private MediaPlayer mp;
    private SurfaceView sv;
    private Button pause;
    private Spinner spinner;
    
    // must stay in sync with mediaplayer.h
    static final int INVOKE_ID_PLAYREADY_DRM = 9900;
    static final int FUN_PROCESS_LICENSE = 1;    
    private MediaPlayer mMediaPlayer;
    private Method mNewRequestMethod;
    private Method mInvokeMethod;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mp=new MediaPlayer();
        sv=(SurfaceView)findViewById(R.id.surfaceView1);
        pause =(Button)findViewById(R.id.pause);
        pause.setOnClickListener(this);
        Button btn = (Button)findViewById(R.id.start);
        btn.setOnClickListener(this);
        btn = (Button)findViewById(R.id.stop);
        btn.setOnClickListener(this);
        btn = (Button)findViewById(R.id.license);
        btn.setOnClickListener(this);
        btn = (Button)findViewById(R.id.debug);
        btn.setOnClickListener(this);
        
        ArrayAdapter<CharSequence> adapter = new ArrayAdapter<CharSequence>(this, android.R.layout.simple_spinner_item);
        // playready encrypted
        adapter.add("http://playready.directtaps.net/smoothstreaming/SSWSS720H264PR/SuperSpeedway_720.ism/Manifest");
        // normal, non encrypted
        adapter.add("http://playready.directtaps.net/smoothstreaming/SSWSS720H264/SuperSpeedway_720.ism/Manifest");
        spinner = (Spinner)findViewById(R.id.spinner);
        spinner.setAdapter(adapter);
        
        mp.setOnCompletionListener(new OnCompletionListener(){

            @Override
            public void onCompletion(MediaPlayer mp) {
                Toast.makeText(MainActivity.this, "video playback complete!", Toast.LENGTH_SHORT).show();
            }
        });
        
        setupInvoke();
    }
    
    @Override
    public void onClick(View v) {
        switch (v.getId())
        {
            case R.id.license:
                doLicense(spinner.getSelectedItem().toString());
                break;
            case R.id.start:
                mp.reset();
                try {
                    //mp.setDataSource("http://playready.directtaps.net/smoothstreaming/SSWSS720H264PR/SuperSpeedway_720.ism/Manifest");
                    String source = spinner.getSelectedItem().toString();
                    mp.setDataSource(source);
                    mp.setDisplay(sv.getHolder());
                    mp.prepare();
                    mp.start();
                    pause.setText("pause");
                    pause.setEnabled(true);
                }catch(IOException e) {
                    e.printStackTrace();
                }
                break;
            case R.id.pause:
                if(mp.isPlaying()){
                    mp.pause();
                    ((Button)v).setText("resume");
                }else{
                    mp.start();
                    ((Button)v).setText("pause");
                }
                break;
            case R.id.stop:
                if(mp.isPlaying()){
                    mp.stop();
                    pause.setEnabled(false);
                }
                break;
            case R.id.debug:
                Log.d(TAG, spinner.getSelectedItem().toString());
                break;
            default:
                break;
        }
    }


    @Override
    protected void onDestroy() {
        if(mp.isPlaying()){
            mp.stop();
        }
        mp.release();
        super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    private String getLAURL(String wrmheader)
    {
        String LA_URL = null;
        try {
            XmlPullParserFactory xmlFactoryObject = XmlPullParserFactory.newInstance();
            XmlPullParser xmlparser = xmlFactoryObject.newPullParser();
            xmlparser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false);
            xmlparser.setInput(new StringReader(wrmheader));

            String text = null;
            int event = xmlparser.getEventType();
            while (event != XmlPullParser.END_DOCUMENT) {
                String name = xmlparser.getName();
                switch (event) {
                    case XmlPullParser.START_TAG:
                        if (name.equals("LA_URL")) {
                            LA_URL = null;
                        }
                        break;

                    case XmlPullParser.TEXT:
                        text = xmlparser.getText();
                        break;

                    case XmlPullParser.END_TAG:
                        if (name.equals("LA_URL")) {
                            Log.d(TAG, "LA_URL:" + text);
                            LA_URL = text;
                        }
                        break;
                }
                event = xmlparser.next();
            }
            Log.d(TAG, "parse xml end.");

        } catch (XmlPullParserException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return LA_URL;
    }
    
    private void setupInvoke()
    {
        // get mediaplayer
        mMediaPlayer = new MediaPlayer();
        Class<MediaPlayer> mpcls = MediaPlayer.class;
        Class<?> params[] = new Class[2];
        params[0] = Parcel.class;
        params[1] = Parcel.class;
        Class<?> no_params[] = null;
        try {
            mMediaPlayer.setDataSource("http://impossible.com/trythis.xml");
            mInvokeMethod = mpcls.getDeclaredMethod("invoke", params);
            if (mInvokeMethod != null) {
                Log.i(TAG, "Got the invoke method: "+mInvokeMethod);
            }
            mNewRequestMethod = mpcls.getDeclaredMethod("newRequest", no_params);
            if (mNewRequestMethod != null) {
                Log.i(TAG, "Got the Newrequest method: " + mNewRequestMethod);
            }
        } catch (Exception x) {
            Log.e(TAG, "Exception: "+x+", "+x.getLocalizedMessage());
        }
    }
    private int ackAndProcessLicense(String wrmheader)
    {
        // do the stuff using mediaplayer invoke
        Log.d(TAG, "ackAndProcessLicense");
        String LA_URL = getLAURL(wrmheader);
        try {
            Parcel request = (Parcel)mNewRequestMethod.invoke(mMediaPlayer, 
                                                              (Object[])null);
            Parcel response = Parcel.obtain();
            request.writeInt(INVOKE_ID_PLAYREADY_DRM);
            request.writeInt(FUN_PROCESS_LICENSE);
            request.writeByteArray(wrmheader.getBytes());
            request.writeByteArray(LA_URL.getBytes());
            mInvokeMethod.invoke(mMediaPlayer, request, response);
            return response.readInt();
        } catch (Exception x) {
            Log.e(TAG, "Exception: "+x+", "+x.getLocalizedMessage());
            return -1;
        }
    }
    
    private void ShowText(final String text) {
        Log.i(TAG, text);
        //Toast.makeText(this, text, Toast.LENGTH_SHORT).show();
        runOnUiThread(new Runnable() {
            public void run() {
              Toast.makeText(MainActivity.this, text, Toast.LENGTH_SHORT).show();
            }
        });
    }
    
    private void doLicense(final String manifest)
    {
        Log.d(TAG, "doLicense");
        Thread thread = new Thread(new Runnable(){
            @Override
            public void run() {
                String WRMHeader = null;
                InputStream stream = null;
                try {
                    
                    // 1. get base64-encoded wrmheader from network
                    URL url = new URL(manifest);
                    HttpURLConnection conn = (HttpURLConnection)url.openConnection();
                 
                    conn.setReadTimeout(10000 /* milliseconds */);
                    conn.setConnectTimeout(15000 /* milliseconds */);
                    conn.setRequestMethod("GET");
                    conn.setDoInput(true);
                    conn.connect();
                 
                    stream = conn.getInputStream();
                    XmlPullParserFactory xmlFactoryObject = XmlPullParserFactory.newInstance();
                    XmlPullParser xmlparser = xmlFactoryObject.newPullParser();
                    
                    xmlparser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false);
                    xmlparser.setInput(stream, null);
                    
                    Log.d(TAG, "parse xml begin.");
                    String ProtectionHeader = null;
                    String text = null;
                    int event = xmlparser.getEventType();
                    while (event != XmlPullParser.END_DOCUMENT) {
                        String name = xmlparser.getName();
                        switch (event) {
                            case XmlPullParser.START_TAG:
                                if (name.equals("ProtectionHeader")) {
                                    ProtectionHeader = null;
                                }
                                break;

                            case XmlPullParser.TEXT:
                                text = xmlparser.getText();
                                break;

                            case XmlPullParser.END_TAG:
                                if (name.equals("ProtectionHeader")) {
                                    //Log.d(TAG, "ProtectionHeader:" + text);
                                    ProtectionHeader = text;
                                }
                                break;
                        }
                        event = xmlparser.next();
                    }
                    Log.d(TAG, "parse xml end.");
                    
                    // 2. base64decode wrmheader
                    if (ProtectionHeader != null) {
                        WRMHeader = new String(Base64.decode(ProtectionHeader, Base64.DEFAULT), "UTF-16LE");
                        WRMHeader = WRMHeader.substring(WRMHeader.indexOf("<WRMHEADER"));
                    }
                    Log.d(TAG, "WRMHeader=" + WRMHeader);
                    
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    if (stream != null) {
                        try {
                            stream.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
                
                // 3. ack and process license using wrmheader. 
                int ret = -1;
                if (WRMHeader != null) {
                    ret = ackAndProcessLicense(WRMHeader);
                }
                
                // 4. show text in screen.
                if (ret == 0) {
                    ShowText("license succeed.");
                } else {
                    ShowText("license failed.");
                }
            }
        });
        thread.start();
    }
}
