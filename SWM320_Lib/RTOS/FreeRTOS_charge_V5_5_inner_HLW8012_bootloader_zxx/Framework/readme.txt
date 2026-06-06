framework
为上层APP提供硬件驱动，软件模块

--------------------------------------------------
FrameWork
- softpack
	--BL1											
		--fk_stdio								
		--fk_console
			---man
			---app:xmodem|TestFont
	--fLibC											
	--FontLib										
	--AppTick										
	--InputMan										
- devices

- drivers


为了编写出平台无关的APP,提出设备和驱动的概念
APP只能操作设备，设备是对驱动的操作封装
设备只是一个虚拟的概念，真正操作的还是
- devices 设备							- drivers 驱动

	
--------------------------------------------------


--------------------------------------------------












