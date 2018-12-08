package com.example.xposeddumpmem;

import android.app.Application;
import android.content.Context;
import android.os.Handler;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage.LoadPackageParam;

public class HookMain implements IXposedHookLoadPackage {

	@Override
	public void handleLoadPackage(LoadPackageParam lpparam) throws Throwable {
		// TODO Auto-generated method stub
		if("com.github.gavin.smid".equals(lpparam.packageName) && "com.github.gavin.smid".equals(lpparam.processName)){
			System.out.println("smid launched");
			System.load("/data/local/tmp/libdumpmem.so");
			
			
			XposedHelpers.findAndHookMethod(Application.class, "attach", Context.class, new XC_MethodHook() {
				@Override
				protected void afterHookedMethod(MethodHookParam param)
						throws Throwable {
					// TODO Auto-generated method stub
					
					new Handler().postDelayed(new Runnable() {
						
						@Override
						public void run() {
							// TODO Auto-generated method stub
							Dumpper.dump("libdu.so", "com.github.gavin.smid");
						}
					}, 10000);
					
				}
			});
			
			
		}
	}

}
