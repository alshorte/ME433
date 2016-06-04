package com.example.amandashorter.camera_app;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

// libraries
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.TextView;
import java.io.IOException;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import android.widget.SeekBar;
import android.widget.TextView;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private Paint paint2 = new Paint();
    private Paint paint3 = new Paint();
    private Paint paint4 = new Paint();
    private Paint paint5 = new Paint();
    private Paint paint6 = new Paint();
    private TextView mTextView;
    private SeekBar myControl;
    private SeekBar myControl2;
    private SeekBar myControl3;
    private TextView myTextView;

    static long prevtime = 0; // for FPS calculation

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceView.getHolder();

        mTextureView = (TextureView) findViewById(R.id.textureview);
        mTextureView.setSurfaceTextureListener(this);

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        paint1.setColor(0xffff0000); // red
        paint1.setTextSize(24);

        paint2.setColor(0xffff0000); // red
        paint2.setTextSize(24);

        paint3.setColor(0xffff0000); // red
        paint3.setTextSize(24);

        paint4.setColor(0xffff0000); // red
        paint4.setTextSize(24);

        paint5.setColor(0xffff0000); // red
        paint5.setTextSize(24);

        paint6.setColor(0xffff0000); // red
        paint6.setTextSize(24);

        myControl = (SeekBar) findViewById(R.id.seek1);
        myControl2 = (SeekBar) findViewById(R.id.seek2);
        myControl3 = (SeekBar) findViewById(R.id.seek3);

    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();        // open camera
        Camera.Parameters parameters = mCamera.getParameters(); // get current properties
        parameters.setPreviewSize(640, 480); // change preview size
        //parameters.setColorEffect(Camera.Parameters.EFFECT_MONO); // black and white
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        mCamera.setParameters(parameters); // set new parameters
        mCamera.setDisplayOrientation(90); // rotate to portrait mode as necessary

        try {
            mCamera.setPreviewTexture(surface);  //take picture and send to screen
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp); // take bmp image of texter view

        final Canvas c = mSurfaceHolder.lockCanvas(); // make canvas in order to look at image
            if (c != null) {

                int[] pixels = new int[bmp.getWidth()]; // make an array of pixels
                int[] pixels2 = new int[bmp.getWidth()]; // make an array of pixels
                int[] pixels3 = new int[bmp.getWidth()]; // make an array of pixels

                int startY = 150; // which row in the bitmap to analyse to read
                int startY2 = 250; // which row in the bitmap to analyse to read
                int startY3 = 350; // which row in the bitmap to analyse to read
                // only look at one row in the image
                bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
                bmp.getPixels(pixels2, 0, bmp.getWidth(), 0, startY2, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
                bmp.getPixels(pixels3, 0, bmp.getWidth(), 0, startY3, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)

                // now we have a 1D array instead of matrix

                // pixels[] is the RGBA data (in black an white).
                // instead of doing center of mass on it, decide if each pixel is dark enough to consider black or white
                // then do a center of mass on the thresholded array
                int[] thresholdedPixels = new int[bmp.getWidth()];
                int[] thresholdedPixels2 = new int[bmp.getWidth()];
                int[] thresholdedPixels3 = new int[bmp.getWidth()];
                int wbTotal = 0; // total mass
                int wbCOM = 0; // total (mass time position)
                int wbTotal2 = 0; // total mass
                int wbCOM2 = 0; // total (mass time position)
                int wbTotal3 = 0; // total mass
                int wbCOM3 = 0; // total (mass time position)
                int thres = myControl.getProgress();
                int thres2 = myControl2.getProgress();
                int thres3 = myControl3.getProgress();

                int OC1RS, OC2RS, lim; //OCR1S control left motor, OC2RS control right motor

                for (int i = 0; i < bmp.getWidth(); i++) {
                    // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                    // if it is greater than some value (600 here), consider it black
                    // play with the 600 value if you are having issues reliably seeing the line
                    if ((red(pixels[i]) > thres) && (green(pixels[i])<thres2) && (blue(pixels[i])< thres3)) { //(255-(red(pixels[i])+green(pixels[i])+blue(pixels[i])) > 600
                        thresholdedPixels[i] = 255; // if the pixel is brown set to on
                        bmp.setPixel(i, startY,0xff000000);
                    }
                    else {
                        thresholdedPixels[i] = 0; // otherwise its not "on" pixel
                    }
                    wbTotal = wbTotal + thresholdedPixels[i];
                    wbCOM = wbCOM + thresholdedPixels[i]*i;

                    if ((red(pixels2[i]) > thres) && (green(pixels2[i])<thres2) && (blue(pixels2[i])< thres3)) { // if the pixel is black save it!
                        thresholdedPixels2[i] = 255;
                        bmp.setPixel(i, startY2,0xff000000);
                    }
                    else {
                        thresholdedPixels2[i] = 0; // otherwise its not "on" pixel
                    }
                    wbTotal2 = wbTotal2 + thresholdedPixels2[i];
                    wbCOM2 = wbCOM2 + thresholdedPixels2[i]*i;

                    if ((red(pixels3[i]) > thres) && (green(pixels3[i])<thres2) && (blue(pixels3[i])< thres3))  { // if the pixel is black save it!
                        thresholdedPixels3[i] = 255;
                        bmp.setPixel(i, startY3,0xff000000);
                    }
                    else {
                        thresholdedPixels3[i] = 0; // otherwise its not "on" pixel
                    }
                    wbTotal3 = wbTotal3 + thresholdedPixels3[i];
                    wbCOM3 = wbCOM3 + thresholdedPixels3[i]*i;
                }
                int COM;
                //watch out for divide by 0
                if (wbTotal<=0) {
                    COM = bmp.getWidth()/2;
                }
                else {
                    COM = wbCOM/wbTotal;
                }

                int COM2;
                //watch out for divide by 0
                if (wbTotal2<=0) {
                    COM2 = bmp.getWidth()/2;
                }
                else {
                    COM2 = wbCOM2/wbTotal2;
                }

                int COM3;
                //watch out for divide by 0
                if (wbTotal3<=0) {
                    COM3 = bmp.getWidth()/2;
                }
                else {
                    COM3 = wbCOM3/wbTotal3;
                }


                // draw a circle where you think the COM is
                canvas.drawCircle(COM, startY, 5, paint1);
                canvas.drawCircle(COM2, startY2, 5, paint2);
                canvas.drawCircle(COM3, startY3, 5, paint3);

                // calculate PWM based on COM2 and COM3
                if (abs(COM2-COM3) < lim) { // if withing a threshold keep going straight
                    OC1RS = ;   // set PWM to 75% of max value
                    OC2RS = ;
                }
                else if(COM2 < COM3) { // path turns left
                    OC1RS =; // set to 50% of max
                    OC2RS =; // set to 100% of max
                }
                else if (COM3 < COM2){ // path turns right
                    OC1RS = ;// set to 100% of max
                    OC2RS = ;// set to 50% of max


                // SEND PWM
                String sendString = String.valueOf(OC1RS) + " " + String.valueOf(OC2RS);
                try {sPort.write(sendString.getBytes(),10); // 10 is the timeout (error)}
                catch (IOException e) {}


                // also write the value as text
                canvas.drawText("COM = " + COM, 10, 200, paint1);
                canvas.drawText("COM2 = " + COM2, 10, 250, paint2);
                canvas.drawText("COM3 = " + COM3, 10, 300, paint3);
                canvas.drawText("wbTotal = " + wbTotal, 10, 350, paint4);
                canvas.drawText("wbTotal2 = " + wbTotal2, 10, 400, paint5);
                canvas.drawText("wbTotal3 = " + wbTotal3, 10, 450, paint6);
                c.drawBitmap(bmp, 0, 0, null);
                mSurfaceHolder.unlockCanvasAndPost(c);

                // calculate the FPS to see how fast the code is running
                long nowtime = System.currentTimeMillis();
                long diff = nowtime - prevtime;
                mTextView.setText("FPS " + 1000/diff);
                prevtime = nowtime;
            }
    }
}


