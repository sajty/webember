package org.installer;

import java.net.*;
import java.io.*;
import java.util.zip.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * This class is created from Installation class.
 * It will run as a separate thread(starting at run() method)
 * Installation class provides the GUI, but this processes the data.
 * 
 * @author sajty
 */
public class InstallationProcess extends Thread{
    public InstallationProcess(Installation install){
        mGui = install;
    }
    private final String mBaseURL = "http://netcologne.dl.sourceforge.net/project/sajty-wf.u/WebEmber/0.1/";
    private final String mReleaseName = "WebEmber-0.1_0";
    Installation mGui;
    private boolean installLinux() {
        boolean is64bit = (mGui.getOS() == 1);
        String arch = is64bit ? "64" : "32";
        String name = mReleaseName+"-Linux_" + arch + "bit";
        String fileName = name + ".tar.bz2";
        String file = mGui.getIntallPath() + "/" + fileName;
        
        boolean success = downloadFile(mBaseURL + fileName, file);
        if(!success){
            return false;
        }
        mGui.log("Extracting " + fileName);
        int ret = this.executeCommand("cd \"" + mGui.getIntallPath() + "\" && tar -xjf " + fileName);
        if(ret != 0){
            return false;
        }
        mGui.log("Extracting succeedded!");
        mGui.log("Moving to install dir...");
        this.executeCommand("cd \"" + mGui.getIntallPath() + "\" && mv -f " + name + "/* ." );
        if(ret != 0){
            return false;
        }
        mGui.log("Moving succeed!");
        mGui.log("Installing plugin...");
        this.executeCommand(mGui.getIntallPath() + "/install.sh");
        if(ret != 0){
            return false;
        }
        return true;
    }
    private boolean installWindows() {
        //http://kent.dl.sourceforge.net/project/sajty-wf.u/WebEmber/0.1/WebEmber-0.1_0.zip
        //String file = downloadFile("http://kent.dl.sourceforge.net/project/sajty-wf.u/WebEmber/0.1/WebEmber-0.1_0.zip");
        String file = mGui.getIntallPath() + "/WebEmber-0.1_0.zip";
        boolean success = downloadFile(
            "http://netcologne.dl.sourceforge.net/project/sajty-wf.u/WebEmber/0.1/WebEmber-0.1_0.zip"
            , file);
        if(!success){
            return false;
        }
        extractZip(file, mGui.getIntallPath() );
        try {
            mGui.log("Registering plugin.");
            Runtime.getRuntime().exec("\"" + mGui.getIntallPath() + "\\install.bat\"");
        } catch(IOException e){
            mGui.log("Unable to register the plugin!");
            mGui.log(stackTraceToString(e));
            
        }
        return true;
    }
    private boolean installMac() {
        String file = System.getProperty("java.io.tmpdir") + "/WebEmber-0.1_0.zip";
        boolean success = downloadFile("http://sajty.elementfx.com/"+mReleaseName + "_OSX.zip", file);
        if(!success){
            return false;
        }
        extractZip(file, "~/Library/Internet Plug-Ins");
        return true;
    }
    @Override
    public void run() {
        boolean success = false;
        try {
            File dir = new File(mGui.getIntallPath());
            mGui.log("Create directory " + dir.getAbsolutePath());
            dir.mkdirs();
            switch(mGui.getOS()){
                case 0:
                case 1:
                    success = installLinux();
                    break;
                case 2:
                    success = installWindows();
                    break;
                case 3:
                    success = installMac();
                    break;
           }
        } catch (RuntimeException e){
            mGui.log(stackTraceToString(e));
        }
        if(success){
            mGui.log("Installation succeeded! You can now play webember!");
        }else{
            mGui.log("Installation failed! Please try the offline installers!");
        }
    }
    private boolean downloadFile(String sURL, String outFile) {
        mGui.log("downloading " + sURL);
        mGui.progressSetValue(0);
        try {
            URL url = new URL(sURL);
            URLConnection connection = url.openConnection();
            HttpURLConnection httpConn = (HttpURLConnection) connection;
            httpConn.setInstanceFollowRedirects(true);
            connection.connect();
            InputStream in = connection.getInputStream();
            FileOutputStream out = new FileOutputStream(outFile);
            int size = connection.getContentLength();
            mGui.progressSetMax(size);
            int i;
            byte b[] = new byte[4096];
            while ((i = in.read(b)) != -1) {
                out.write(b,0,i);
                mGui.progressAddValue(i);
            }
            out.close();
            return true;
        } catch (Exception e) {
            mGui.log("Failed to download file.");
            mGui.log(stackTraceToString(e));
        }
        return false;
    }
    private void copyFile(String srFile, String dtFile) {
        mGui.log("copying " + srFile + " to " + dtFile);
        try {
            File f1 = new File(srFile);
            File f2 = new File(dtFile);
            InputStream in = new FileInputStream(f1);
  
            //For Overwrite the file.
            OutputStream out = new FileOutputStream(f2);

            byte[] buf = new byte[1024];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            in.close();
            out.close();
            mGui.log("File copied.");
        } catch(FileNotFoundException ex) {
            mGui.log(ex.getMessage() + " in the specified directory.");
        } catch(IOException e) {
            mGui.log(stackTraceToString(e));
        }
    }
    //src:
    public int executeCommand(String cmd) {
        int ret = 1;
        mGui.log("Running: " + cmd);
        Runtime run = Runtime.getRuntime();
        Process pr = null;
        String tmpscript=mGui.getIntallPath() + "/tmp.sh";
        try {
            PrintWriter out = new PrintWriter(new FileWriter(tmpscript));
            out.println("#! /bin/bash");
            out.println("set -e");
            out.println(cmd);
            out.close();
            pr = run.exec("chmod +x " + tmpscript);
            pr = run.exec(tmpscript);
        } catch (IOException e) {
            mGui.log(stackTraceToString(e));
        }
        try {
            ret = pr.waitFor();
        } catch (InterruptedException e) {
            mGui.log(stackTraceToString(e));
        }
        BufferedReader buf = new BufferedReader(new InputStreamReader(pr.getInputStream()));
        String line = "";
        try {
            while ((line=buf.readLine())!=null) {
                mGui.log(line);
            }
        } catch (IOException e) {
            mGui.log(stackTraceToString(e));
        }
        return ret;
    }
    
    public void extractZip(String src, String dst) {
        mGui.log("extracting " + src + " to " + dst);
        try {
            byte[] buf = new byte[4096];
            ZipInputStream is = new ZipInputStream(new FileInputStream(src));
            ZipEntry zipentry;
            while ((zipentry = is.getNextEntry()) != null) {
                String entryName = zipentry.getName();
                mGui.log("extracting: " + entryName);
                if(zipentry.isDirectory()){
                    File dir = new File(dst + "/" + entryName);
                    dir.mkdir();
                }else{
                    FileOutputStream file = new FileOutputStream(dst + "/" + entryName);
                    BufferedOutputStream out = new BufferedOutputStream(file);
                    int n;
                    while ((n = is.read(buf, 0, 4096)) > -1){
                        out.write(buf, 0, n);
                    }
                    out.close();
                    is.closeEntry();
                }
            }
            is.close();
        } catch (Exception e) {
            mGui.log("Unable to extract archive!");
            mGui.log(stackTraceToString(e));
        }
    }
    public String stackTraceToString(Throwable e) {
        String retValue = null;
        StringWriter sw = null;
        PrintWriter pw = null;
        try {
            sw = new StringWriter();
            pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            retValue = sw.toString();
        } finally {
            try {
                if(pw != null)  pw.close();
                if(sw != null)  sw.close();
            } catch (IOException ignore) {}
        }
        return retValue;
    }
}
