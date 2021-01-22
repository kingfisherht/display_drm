对framebuffer操作不能直接加载驱动i915,否则看不到现象

在机器cmdline加入参数: i915.modeset=0 video=efifo:off

启动机器后：
        执行./a.out
能看到颜色在屏幕上显示

