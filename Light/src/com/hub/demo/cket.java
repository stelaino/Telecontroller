package com.hub.demo;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class cket {
    public void open() throws UnknownHostException, IOException {
    	Socket socket=new Socket("localhost",10086);
    	 OutputStream os=socket.getOutputStream();//字节输出流
         PrintWriter pw=new PrintWriter(os);//将输出流包装为打印流
         pw.write("1");
         pw.flush();
         pw.close();
         os.close();
         socket.close();
    }
    public void close() throws UnknownHostException, IOException {
    	Socket socket=new Socket("localhost",10086);
    	 OutputStream os=socket.getOutputStream();//字节输出流
         PrintWriter pw=new PrintWriter(os);//将输出流包装为打印流
         pw.write("2");
         pw.flush();
         pw.close();
         os.close();
         socket.close();
    }
}