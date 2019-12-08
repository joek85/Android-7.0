package com.example.audiorecordwav;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.PixelFormat;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaRecorder;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;


public class MainActivity extends Activity {
	
	private static String TAG = "AudioRecordWav";
	private int audioSource = MediaRecorder.AudioSource.CAMCORDER;
	private static int sampleRateInHz = 16000;
	private static int channelConfig = AudioFormat.CHANNEL_IN_MONO;
	private static int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
	private int bufferSizeInBytesAudioRecord = 0;
	private Button mReplayBtn;
	private Button Stop;
	private AudioRecord audioRecord;
	private boolean isRecord = false;
	private static final String AudioName = "/sdcard/orig.raw";
	private static final String NewAudioName = "/sdcard/new.wav";
	
	private boolean keyLongPressFlag = false;
	
	private static final boolean GOOGLEVOICE = false;
	private static boolean VoiceKeyDown = false;
	
	
	InputStream fis;
    OutputStream fos;
    
    private MediaPlayer mPlayer = null;
    private ProgressBar mProgressBar = null;
    private ImageView mVoiceImg = null;
    private TextView mHintText = null;
    private ProgressDialog recordProgressDialog = null;
    private PlayingThread workThread;
    
    private static final String BUTTON_TEXT_RECORDING = "RECORDING NOW";
    private static final String BUTTON_TEXT_REPLAY = "REPLAY";    
    private static final String BUTTON_TEXT_REPLAYING = "REPLAYING NOW";
    
    private static final int MSG_RECORD_STARTED = 1;
    private static final int MSG_RECORD_STOPPED = 2;
    private static final int MSG_RECORD_TIMEOUT = 3;
    private static final int MSG_PLAY_STARTED = 4;
    private static final int MSG_PLAY_STOPPED = 5;
	
    private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			switch (msg.what) {
				case MSG_RECORD_STARTED:
					Log.d(TAG, "thread " + Thread.currentThread().getId() + " : OK, just mReplayBtn your work");
					if(mReplayBtn != null) {
						mReplayBtn.setText(BUTTON_TEXT_RECORDING);
						mReplayBtn.setEnabled(false);
					}
					
					if(mProgressBar != null)
						mProgressBar.setVisibility(View.VISIBLE);
					break;
					
				case MSG_RECORD_STOPPED:
					Log.d(TAG, "stop msg");

					if(mProgressBar != null)
						mProgressBar.setVisibility(View.INVISIBLE);
				
					if(mReplayBtn != null) {
						mReplayBtn.setEnabled(true);
						mReplayBtn.setText(BUTTON_TEXT_REPLAY);
					}
				
					stopRecord();
					break;
				case MSG_RECORD_TIMEOUT:
					Log.d(TAG, "time to stop recording");
					isRecord = false;
					break;
				case MSG_PLAY_STARTED:
					Log.d(TAG, "thread " + Thread.currentThread().getId() + " : OK, just mReplayBtn your work");
					if(mReplayBtn != null) {
						mReplayBtn.setEnabled(false);
						mReplayBtn.setText(BUTTON_TEXT_REPLAYING);
					}
					break;
					
				case MSG_PLAY_STOPPED:
					if(mReplayBtn != null) {
						mReplayBtn.setText(BUTTON_TEXT_REPLAY);
						mReplayBtn.setEnabled(true);
					}
					break;
					
				default:
					Log.d(TAG, "Unknown Message !!!");
			}
			super.handleMessage(msg);
		}
    	
    };
    
    private class LocalMPOnCompletionListener implements OnCompletionListener {

		@Override
		public void onCompletion(MediaPlayer mp) {
			// TODO Auto-generated method stub
			Log.d(TAG, "release the MediaPlayer!!!");
			mp.release();
			//mReplayBtn.setEnabled(true);
			if(mHandler != null) {
				Message msg = mHandler.obtainMessage(MSG_PLAY_STOPPED);
				mHandler.sendMessage(msg);
			}
		}
    	
    }
    
    private class PlayingThread extends Thread {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			mPlayer = new MediaPlayer();
			if(mPlayer != null) {
				
				Message msg = mHandler.obtainMessage(MSG_PLAY_STARTED);
				mHandler.sendMessage(msg);
				
				try {
					mPlayer.setOnCompletionListener( new LocalMPOnCompletionListener() );
					mPlayer.setDataSource(NewAudioName);
					Log.d(TAG, "filename: "+ NewAudioName);
					mPlayer.prepare();
					mPlayer.start();
				} catch (IOException e) {
					Log.e(TAG, "prepare() failed");
				}
			}else
				Log.d(TAG, "MediaPlayer is null!");
			
			
		}
    	
    }
    
    
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch(keyCode)
		{
		case KeyEvent.KEYCODE_F5:
			//Log.d(TAG, "key press F5" + event.getRepeatCount());
			if (event.getRepeatCount() == 0)
			{
				Log.d(TAG, "Thread num: " + Thread.currentThread().getId());
				//Log.d(TAG, "mReplayBtn Tracking");
				
				//recordProgressDialog.show();
				
				startRecord();

			} else {
				VoiceKeyDown = true;
			}
			break;
		default:
			Log.d(TAG, "key short press" + event.getRepeatCount());
			break;
		}
		return super.onKeyDown(keyCode, event);
	}
/*
	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch(keyCode)
		{
		case KeyEvent.KEYCODE_F5:
			Log.d(TAG, "key long press F5, mReplayBtn recording");
			mReplayBtnRecord();
			keyLongPressFlag = true;
			return true;
		default:
			Log.d(TAG, "key long press");
			break;
		}
		return super.onKeyLongPress(keyCode, event);
	}
*/
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch(keyCode)
		{
		case KeyEvent.KEYCODE_F5:
			Log.d(TAG, "key release F5");
			if (!keyLongPressFlag)
			{
//				if(mProgressBar != null)
//					mProgressBar.setVisibility(View.INVISIBLE);
//				
//				Log.d(TAG, "Thread num: " + Thread.currentThread());
//				if(mReplayBtn != null) {
//					mReplayBtn.setEnabled(true);
//					mReplayBtn.setText(BUTTON_TEXT_REPLAY);
//				}
				
//				stopRecord();
				isRecord = false;
				keyLongPressFlag = false;
			}
			
			break;
		default:
			Log.d(TAG, "key release");
			break;
		}
		
		return super.onKeyUp(keyCode, event);
	}

	

	public void onCreate(Bundle savedInstanceState) {
		
		getWindow().setFormat(PixelFormat.TRANSLUCENT);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);
		init();
		Log.d(TAG, "Thread num: " + Thread.currentThread());
		mVoiceImg = (ImageView)findViewById(R.id.imageView1);
		if(mVoiceImg!=null) {
			double mWidth = mVoiceImg.getScaleX();
			double mHeight = mVoiceImg.getScaleY();
		}
		
		mHintText = (TextView)findViewById(R.id.textView1);
		if(mHintText != null)
			mHintText.setText("Press And Hold");
		super.onCreate(savedInstanceState);
	}

	private void init() {
		mReplayBtn = (Button) this.findViewById(R.id.start);
		mReplayBtn.setOnClickListener(new TestAudioListener());
		mProgressBar = (ProgressBar)this.findViewById(R.id.progressBar1);
		mProgressBar.setVisibility(View.INVISIBLE);
		/*
		recordProgressDialog = new ProgressDialog(MainActivity.this);
		recordProgressDialog.setTitle("AudioRecordWav");
		recordProgressDialog.setMessage("Recording Now ...");
		recordProgressDialog.setCancelable(true);
		*/
		
		//creatAudioRecord();
	}

	private void creatAudioRecord() {
		bufferSizeInBytesAudioRecord = AudioRecord.getMinBufferSize(sampleRateInHz,
				channelConfig, audioFormat);
		Log.e(TAG, "AudioRecord getMinBufferSize"+bufferSizeInBytesAudioRecord);
		audioRecord = new AudioRecord(audioSource, sampleRateInHz,
				channelConfig, audioFormat, bufferSizeInBytesAudioRecord * 10);
		
		if (audioRecord == null)
			Log.d(TAG, "can't open audioRecord");
		
	}

	class TestAudioListener implements OnClickListener {

		@Override
		public void onClick(View v) {
			Log.d(TAG, "mReplayBtn playing!!!");
			if (v == mReplayBtn) {
				if(mHandler != null) {
					Message msg = mHandler.obtainMessage(MSG_PLAY_STARTED);
					mHandler.sendMessage(msg);
				}
				mPlayer = new MediaPlayer();
		        try {
		        	mPlayer.setOnCompletionListener( new LocalMPOnCompletionListener() );
		            mPlayer.setDataSource(NewAudioName);
		            Log.d(TAG, "filename: "+ NewAudioName);
		            mPlayer.prepare();
		            mPlayer.start();
		        } catch (IOException e) {
		            Log.e(TAG, "prepare() failed");
		        }
			}
			if (v == Stop) {
				 mPlayer.release();
			        mPlayer = null;
			}

		}


	}

	private void startRecord() {
		while(audioRecord == null)
			creatAudioRecord();
		//new Thread(new AudioRecordThread()).mReplayBtn();
		if(mHandler != null) {
			Message msg = mHandler.obtainMessage(MSG_RECORD_STARTED);
			mHandler.sendMessage(msg);
		}
		audioRecord.startRecording();
		isRecord = true;

		new Thread(new AudioRecordThread()).start();
		
	}

	private void stopRecord() {
		if (audioRecord != null) {
			Log.d(TAG, "Stop record");
			audioRecord.stop();
			audioRecord.release();
			audioRecord = null;
		}
		
		//mHandler.sendMessage(mHandler.obtainMessage(MSG_STOP_RECORD));
		
		/*
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		*/
		workThread = new PlayingThread();
		workThread.start();
		
		//if(mPlayer == null) {

//		mPlayer = new MediaPlayer();
//		try {
//			mPlayer.setOnCompletionListener(new OnCompletionListener() {
//				
//				@Override
//				public void onCompletion(MediaPlayer mp) {
//					// TODO Auto-generated method stub
//					Log.d(TAG, "release the MediaPlayer!!!");
//					mp.release();
//					mReplayBtn.setEnabled(true);
//				}
//			});
//			mPlayer.setDataSource(NewAudioName);
//			Log.d(TAG, "filename: "+ NewAudioName);
//			mPlayer.prepare();
//			mPlayer.mReplayBtn();
//		} catch (IOException e) {
//			Log.e(TAG, "prepare() failed");
//		}
		
		//}
	}

	private void close() {
		if (audioRecord != null) {
			System.out.println("stopRecord");
			isRecord = false;
			audioRecord.stop();
			audioRecord.release();
			audioRecord = null;
		}
		
	}
	
	private class TimerThread implements Runnable {

		@Override
		public void run() {
			// TODO Auto-generated method stub
			try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			Log.d(TAG, "recording timeout, stop it!");
			Message msg = mHandler.obtainMessage(MSG_RECORD_TIMEOUT);
			mHandler.sendMessage(msg);
		}
		
	}

	class AudioRecordThread implements Runnable {
		@Override
		public void run() {
			Log.d(TAG, "Thread enter");
			writeDateTOFile();
			copyWaveFile(AudioName, NewAudioName);
			Message msg = mHandler.obtainMessage(MSG_RECORD_STOPPED);
			mHandler.sendMessage(msg);
			Log.d(TAG, "recording stopped!!!");
		}
	}

	private void writeDateTOFile() {
		byte[] audiodata = new byte[bufferSizeInBytesAudioRecord * 2];
		FileOutputStream fos = null;
		int readsize = 0;
		long totalSize = 0;
		try {
			File file = new File(AudioName);
			if (file.exists()) {
				file.delete();
			}
			fos = new FileOutputStream(file);
		} catch (Exception e) {
			e.printStackTrace();
		}
		while (isRecord == true) {

			//readsize = audioRecord.read(audiodata, 0, bufferSizeInBytesAudioRecord * 2);
			readsize = audioRecord.read(audiodata, 0, bufferSizeInBytesAudioRecord * 2);
			totalSize += readsize;

			//Log.d(TAG, "readsize: " + readsize);
			
			if (AudioRecord.ERROR_INVALID_OPERATION != readsize) {
				try {
					fos.write(audiodata);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			
		}
		
		Log.d(TAG, "total size: " + totalSize);
		try {
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void copyWaveFile(String inFilename, String outFilename) {
		FileInputStream in = null;
		FileOutputStream out = null;
		long totalAudioLen = 0;
		long totalDataLen = totalAudioLen + 36;
		long longSampleRate = sampleRateInHz;
		int channels = 1;
		long byteRate = 16 * sampleRateInHz * channels / 8;
		byte[] data = new byte[bufferSizeInBytesAudioRecord];
		try {
			in = new FileInputStream(inFilename);
			out = new FileOutputStream(outFilename);
			totalAudioLen = in.getChannel().size();
			totalDataLen = totalAudioLen + 36;
			WriteWaveFileHeader(out, totalAudioLen, totalDataLen,
					longSampleRate, channels, byteRate);
			while (in.read(data) != -1) {
				out.write(data);
			}
			in.close();
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	private void WriteWaveFileHeader(FileOutputStream out, long totalAudioLen,
			long totalDataLen, long longSampleRate, int channels, long byteRate)
			throws IOException {
		byte[] header = new byte[44];
		header[0] = 'R'; // RIFF/WAVE header
		header[1] = 'I';
		header[2] = 'F';
		header[3] = 'F';
		header[4] = (byte) (totalDataLen & 0xff);
		header[5] = (byte) ((totalDataLen >> 8) & 0xff);
		header[6] = (byte) ((totalDataLen >> 16) & 0xff);
		header[7] = (byte) ((totalDataLen >> 24) & 0xff);
		header[8] = 'W';
		header[9] = 'A';
		header[10] = 'V';
		header[11] = 'E';
		header[12] = 'f'; // 'fmt ' chunk
		header[13] = 'm';
		header[14] = 't';
		header[15] = ' ';
		header[16] = 16; // 4 bytes: size of 'fmt ' chunk
		header[17] = 0;
		header[18] = 0;
		header[19] = 0;
		header[20] = 1; // format = 1
		header[21] = 0;
		header[22] = (byte) channels;
		header[23] = 0;
		header[24] = (byte) (longSampleRate & 0xff);
		header[25] = (byte) ((longSampleRate >> 8) & 0xff);
		header[26] = (byte) ((longSampleRate >> 16) & 0xff);
		header[27] = (byte) ((longSampleRate >> 24) & 0xff);
		header[28] = (byte) (byteRate & 0xff);
		header[29] = (byte) ((byteRate >> 8) & 0xff);
		header[30] = (byte) ((byteRate >> 16) & 0xff);
		header[31] = (byte) ((byteRate >> 24) & 0xff);
		header[32] = (byte) (2 * 16 / 8); // block align
		header[33] = 0;
		header[34] = 16; // bits per sample
		header[35] = 0;
		header[36] = 'd';
		header[37] = 'a';
		header[38] = 't';
		header[39] = 'a';
		header[40] = (byte) (totalAudioLen & 0xff);
		header[41] = (byte) ((totalAudioLen >> 8) & 0xff);
		header[42] = (byte) ((totalAudioLen >> 16) & 0xff);
		header[43] = (byte) ((totalAudioLen >> 24) & 0xff);
		out.write(header, 0, 44);
	}

	@Override
	protected void onDestroy() {
		close();
		super.onDestroy();
	}
}
