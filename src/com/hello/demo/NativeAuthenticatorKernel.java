package com.hello.demo;

/**
 * Created by jiamei on 6/26/15.
 */
public class NativeAuthenticatorKernel
{
    static
    {
         System.loadLibrary("mtee");
         System.loadLibrary("teeclientdemo");
    }
    
    public static native byte[] bigIntAdd();
}
