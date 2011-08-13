package org.installer;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * Installation.java
 *
 * Created on 2011.08.12., 9:06:02
 */
/**
 *
 * @author sajty
 */
import java.net.*;
import java.io.*;
import java.util.zip.*;
public class Installation extends javax.swing.JPanel {

    /** Creates new form Installation */
    public Installation(org.installer.AppMain _app) {
        app = _app;
        initComponents();
        installThread = new InstallThread();
        installThread.start();
    }
    class InstallThread extends Thread {
        public void run() {
            switch(app.getOS()){
            case 0:
                installLinux();
                break;
            case 1:
                installWindows();
                break;
            case 2:
                installMac();
                break;
            }
            
        }
    }
    private final String ReleaseName = "WebEmber-0.1";
    private final int Release = 0;
    private void installLinux() {
        prgInstall.setMaximum(300);
        String file = downloadFile("http://sajty.elementfx.com/"+ReleaseName+"."+Release+"_LINUX.zip");
        if(file != "") {
            extractZip(file, app.getIntallPath() );
            copyFile(app.getIntallPath() + "lib/npWebEmber.so", "~/.mozilla/plugins/npWebEmber.so");
        }
    }
    private void installWindows() {
        prgInstall.setMaximum(200);
        String file = downloadFile("http://sajty.elementfx.com/"+ReleaseName+"."+Release+"_WIN32.zip");
        if(file != ""){
            extractZip(file, app.getIntallPath() );
            try {
                Runtime.getRuntime().exec("regsvr32 " + app.getIntallPath() + "/bin/WebEmber.dll");
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    private void installMac(){
        prgInstall.setMaximum(200);
        String file = downloadFile("http://sajty.elementfx.com/"+ReleaseName+"."+Release+"_OSX.zip");
        if(file != ""){
            extractZip(file, "~/Library/Internet Plug-Ins");
        }
    }
    private void progressIncrement(int max){
        int val = prgInstall.getValue();
        if(val<max){
            prgInstall.setValue(val + 1);
        }
    }
    private void log(String s){
        txtInstallLog.append(s + "\n");
    }
    private String downloadFile(String sURL) {
        log("downloading " + sURL);
        int limit = prgInstall.getValue();
        try {
            URL url = new URL(sURL);
            URLConnection connection = url.openConnection();

            InputStream in = connection.getInputStream();
            FileOutputStream out = new FileOutputStream(url.getFile());
            int i;
            byte b[] = new byte[4096];
            while ((i = in.read(b)) != -1) {
                out.write(b,0,i);
                progressIncrement(limit);
            }
            out.close();
            return url.getFile();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
    private void copyFile(String srFile, String dtFile){
        log("copying " + srFile + " to " + dtFile);
        int limit = prgInstall.getValue()+100;
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
                progressIncrement(limit);
            }
            in.close();
            out.close();
            System.out.println("File copied.");
        } catch(FileNotFoundException ex) {
            System.out.println(ex.getMessage() + " in the specified directory.");
        } catch(IOException e) {
            e.printStackTrace();
        }
    }
    public void extractZip(String src, String dst) {
        log("extracting " + src + " to " + dst);
        int limit = prgInstall.getValue()+100;
        try {
            byte[] buf = new byte[4096];
            ZipInputStream is = new ZipInputStream(new FileInputStream(src));

            ZipEntry zipentry;
            while ((zipentry = is.getNextEntry()) != null) {
                progressIncrement(limit);
                String entryName = zipentry.getName();
                System.out.println("extracting: " + entryName);
                
                FileOutputStream file = new FileOutputStream(dst + "/" + entryName);
                BufferedOutputStream out = new BufferedOutputStream(file);
                int n;
                while ((n = is.read(buf, 0, 4096)) > -1){
                    out.write(buf, 0, n);
                }
                out.close(); 
                is.closeEntry();
            }
            is.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        prgInstall = new javax.swing.JProgressBar();
        jButton3 = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        txtInstallLog = new javax.swing.JTextArea();

        jButton3.setText("abort");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });

        txtInstallLog.setColumns(20);
        txtInstallLog.setEditable(false);
        txtInstallLog.setRows(5);
        jScrollPane1.setViewportView(txtInstallLog);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap(167, Short.MAX_VALUE)
                .addComponent(jButton3)
                .addGap(174, 174, 174))
            .addGroup(layout.createSequentialGroup()
                .addGap(34, 34, 34)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 327, Short.MAX_VALUE)
                    .addComponent(prgInstall, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, 327, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(39, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addGap(27, 27, 27)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 189, Short.MAX_VALUE)
                .addGap(18, 18, 18)
                .addComponent(prgInstall, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jButton3)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton3ActionPerformed
// TODO add your handling code here:
}//GEN-LAST:event_jButton3ActionPerformed

    private final AppMain app;
    private Thread installThread;
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButton3;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JProgressBar prgInstall;
    private javax.swing.JTextArea txtInstallLog;
    // End of variables declaration//GEN-END:variables
}
