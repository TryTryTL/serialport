package org.qtproject.example;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class UsbController {

    private Context context;
    private UsbManager usbManager;
    private PendingIntent permissionIntent;
    private UsbSerialPort pendingUsbSerialPort; // 用于存储等待打开的串口

    private final BroadcastReceiver usbPermissionReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if ("org.qtproject.example.USB_PERMISSION".equals(action)) {
                synchronized (this) {
                    UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        if (device != null && pendingUsbSerialPort != null && pendingUsbSerialPort.getDriver().getDevice().equals(device)) {
                            // 如果用户授予了权限，打开串口
                            openSerialPort(pendingUsbSerialPort, 9600, UsbSerialPort.DATABITS_8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
                        }
                    } else {
                        Log.d("USB", "Permission denied for device " + device);
                    }
                    pendingUsbSerialPort = null; // 重置 pendingUsbSerialPort
                }
            }
        }
    };

    public UsbController(Context context) {
        this.context = context;
        this.usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        this.permissionIntent = PendingIntent.getBroadcast(context, 0, new Intent("org.qtproject.example.USB_PERMISSION"), 0);

        // 注册广播接收器
        IntentFilter filter = new IntentFilter("org.qtproject.example.USB_PERMISSION");
        context.registerReceiver(usbPermissionReceiver, filter);
    }

    public String[] getDeviceList() {
        HashMap<String, UsbDevice> usbDevices = usbManager.getDeviceList();
        String[] deviceNames = new String[usbDevices.size()];
        int index = 0;
        for (UsbDevice device : usbDevices.values()) {
            deviceNames[index++] = device.getDeviceName();
            usbManager.requestPermission(device, permissionIntent);  // 请求权限
        }
        return deviceNames;
    }

    public List<UsbSerialPort> getAllSerialPort() {
        List<UsbSerialDriver> drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager);
        List<UsbSerialPort> result = new ArrayList<>();
        for (UsbSerialDriver driver : drivers) {
            List<UsbSerialPort> ports = driver.getPorts();
            result.addAll(ports);
        }
        return result;
    }

    public void requestPermissionAndOpenPort(UsbSerialPort usbSerialPort, int baudRate, int dataBits, int stopBits, int parity) {
        UsbDevice device = usbSerialPort.getDriver().getDevice();
        if (usbManager.hasPermission(device)) {
            openSerialPort(usbSerialPort, baudRate, dataBits, stopBits, parity);
        } else {
            pendingUsbSerialPort = usbSerialPort; // 将串口保存为等待状态
            usbManager.requestPermission(device, permissionIntent);
        }
    }

    private void openSerialPort(UsbSerialPort usbSerialPort, int baudRate, int dataBits, int stopBits, int parity) {
        UsbDeviceConnection connection = usbManager.openDevice(usbSerialPort.getDriver().getDevice());
        if (connection != null) {
            try {
                usbSerialPort.open(connection);
                usbSerialPort.setParameters(baudRate, dataBits, stopBits, parity);
                Log.d("USB", "Serial port opened successfully.");
            } catch (IOException e) {
                Log.e("USB", "Error opening serial port: " + e.getMessage());
            }
        } else {
            Log.e("USB", "Could not open connection - permission might be required.");
        }
    }
}
