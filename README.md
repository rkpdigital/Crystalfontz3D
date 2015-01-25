# Crystalfontz3D
Crystalfontz3D is to be a port of TinyG2 that runs on the CrystalFontz 10049 board with the 10036 SOM
  and Linux to build a high performance 3 axis motion control system.

    1. Running as a user application in Linux with all Linux's system support available.
    2. Using ARM specific FIQ hardware that is unused by Linux for real time operations.
    3. Communication and control is from a console and from gcode files.
      A, This console can be commandeered by the UI program for a GUI user interface.
    4. Output is to FIQ10049 and slowCmd files for use by another user application that loads
       the FIQ steps file. (slowCmd file not yet implemented.)
    5. Status output is to the console.
    6. Latest System Configuration is to a file instead of ARM NVM.
    7. Versions to run on a desktop Linux system or on the 10036/10049 SOM and printer
       controller board.
    8. Much of the TinyG2 code is mostly unchanged except stepper, switches, and hardware.
       (PWM and spindle are left out for now.)
    9. Crystalfontz3D doesn't use any interrupts. Another application transfers the FIQ steps
       file to the FIQ shared memory for real time control of the 3D printer from the fast
       FIQ interrupt.

The license is:
                    GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991

TinyG2 is an ARM Port of the [TinyG Motion control system]
By:
 * Copyright (c) 2013 Alden S. Hart, Jr.
 * Copyright (c) 2013 Robert Giseburt

Refer to the following for more information:
(https://github.com/synthetos/TinyG)
[(wiki)](https://github.com/synthetos/TinyG/wiki)
[G2 wiki](https://github.com/synthetos/g2/wiki)

G2 has a number of advanced features, including:
* Full 6 axis motion control - XYZ linear axes and ABC rotary axes
* Jerk controlled motion for acceleration planning (S curve 3rd order motion planning)
* RESTful interface using JSON (Removed in Crystalfontz3D.)
* Complete status and system state displays



Versions:
10049G2LEG - Old code for use with the compiled in FIQ code that expecte 2 12byte commands for each step.

10049G2FIQ - New FIQ structure with 1 8byte command and a state machine in the FIQ to do 2 interrupts.
             cfa10049_fiq.hand stepper.cpp are the major changes along with the cfa10049_new_fiq (TBD)
             loadable module. See "Proposed Small file FIQ"

Version 2.0 dated 08/20/14 - FIQ and LEG versions - Also uses MOTOR_STATE more effectivly in stepper.cpp for
                                                    _load_move, and _output_to_FIQ to speed execution up by 33%.

This is a snapshot of unit tested code and the output file was desk checked to be correct.



The Lathe/Mill/3D Printer hardware configuration is defined in an include file selected in settings.h.
The program must be custom compiled for each machine.
Look for MACHINE PROFILES. The default is - settings_3DPrint.h. 



Compile for Linux PC:
1. Install the gcc compiler for the PC then - sudo apt-get install cmake-qt-gui
2. cd PC-build/
   cmake -DCMAKE_BUILD_TYPE=DEBUG ..
   make



Cross Compile for 10049:

1. Get and Install CodeSourcery.
  A. Change the permissions and go the the BASH shell (dpkg-reconfigure -plow bash).
     (OR - sudo ln -sf bash /bin/sh - Then sudo ln -sf bash /bin/sh to revert.)

  B.Get it from - https://sourcery.mentor.com/GNUToolchain/package11449/public/arm-none-linux-gnueabi
                  /arm-2013.05-24-arm-none-linux-gnueabi.bin
    1.Or look in - http://www.mentor.com/embedded-software/codesourcery (Get the GNU/Linux release)

  C. From a Command Line Terminal window - sudo arm-2013.05-24-arm-none-linux-gnueabi.bin
    1. Follow the instructions in the installer window.
      a. Remember/record the path where the tools are installed.
      b. Add this to the path of the startup from/for a command line terminal window.
        1. In .bashrc, add - PATH="/root/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin:$PATH"
                       and - alias sudo='sudo env PATH=$PATH'
                       

2. Get the library images from the Target embedded device.
  A.Get a direct copy of the /usr/include and /usr/lib directories from the CFA-921
    1. Connect USB on Linux Devbox to the micro B USB connector on the SOM.
     
    2. Using ssh file transfer from the CFA-921 to Linux. 
      a. From the Gnome Desktop:
         Places > Connect to Server.
         Server: 192.168.42.1 - Port 22
         Type: SSH
         User name: root
         Password: root
  
      b. From the Unity Desktop:
         From the Top "Dash Home" icon: Or Press Alt+F2 and type nautilus into "Search" 
         When it comes up:  go to File > Browse Network and select the cfa100xx icon.
         If this doesn't work the backend is probably not installed.
  
      c. From Windows with WinSCP:
         The filenames will probably be maqngled and case is ignored. Not useful.

      d. If you have a problem it's probably that the registered SSH key for this IP address is unknown for
         this user on the desktop or is for a different SOM or SD card.
         1. If a window comes up asking if you can trust this new server, hit the "yes" button.
         2. Try - ssh 192.168.42.1 - from a Command Line Terminal window to verify the problem.
           a. Then - ssh-keygen -f "/home/username/ or /root/  .ssh/known_hosts" -R 192.168.42.1
           b. Sometimes it's best to manually delete all entries in /home/rkpdigital/.ssh/known_hosts.
            
    3. Create a CFA-921-usr folder in the working folder.

    4. Copy /usr/lib and /usr/include directories from the SOM to the CFA-921-usr folder.
      a. Use the Nautilus GUI to right click and "Copy" the source folder.
      b. Use the Naulilus GUI from within the CFA-921-usr folder in the working folder to right click and "Paste" to
         the destination.
      c. May have Microsoft .dll file copy errors. - Just skip those files.

    5. Copy additional files from the SOM.
      a. Copy /lib/libpthread-2.18.so from  from the SOM to/CFA-921-usr/lib
      b. Copy /lib/libc-2.18.so and /lib/ld-2.18.so from the SOM to/CFA-921-usr/lib

  B. Modify the text based library redirect files.
    1. Modify CFA-921-usr/lib/libpthread.so to point to /home/UserName/Projects/CrystalFontzRambo/CFA-921-usr/lib/
       as a prefix for each of the following:
       libpthread-2.18.so , and libpthred_nonshared.a.
       a. In the nautilus folder GUI; Right click and "Open with Text Editor", or - 
          double left click on the text of the file name or - 
          right click and "Open With Other Application" and select gedit. 
       
    2. Modify CFA-921-usr/lib/libc.so to point to /home/UserName/Projects/CrystalFontzRambo/CFA-921-usr/lib/ as a
       prefix for each of the following:
       libc-2.18.so (Replaces libc.so.6), libc_nonshared.a , and ld-2.18.so(Replaces ld-linux.so.3).

  C. Create soft links in CFA-921-usr/lib:
    1. Create a soft link called libudev.so.0 that points to libudev.so
      a. From the nautilus folder GUI; Right click on libudif.so and "Make Link"
      b. In the nautilus folder GUI; Find "Link to libudev.so"
      c. From the nautilus folder GUI; Right click on "Link to libudev.so" and "Rename" it to "libudev.so.0".
    2. Create a soft link called libz.so.1 that points to libz.so
    3. Create a soft link called libQtSerialPortE.so that points to libQtSerialPortE.so.1

3. Edit the "codesourcery_toolchain.cmake" file to point to your tools and libraries.

4. cd ARM-build/
   cmake -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_TOOLCHAIN_FILE=codesourcery_toolchain.cmake ..
   make



Operation:
1. The input "G" code file should contain the following instructions to set up limits.

   F3600 ( The program will error out without a limit loaded.)

2. ./10049G2 -g <Input "G" code file> -f <output FIQ code file>

3. Must Ctrl-c to exit the program after the conversion is complete.
     There isn't much progress reporting (Once every 512 lines of "G" code.). 



Proposed Small file FIQ:
 *    One 8byte shared/FIQ/File location per motor step. (Instead of the 2x12 byte steps for the Legacy FIQ.)
 *    Loadable FIQ module with state machine to step motors from compressed FIQ memory structure.
 *    // State = 0 Read data, Clear Steps, Set direction, Set timer to 1.
 *    // State = 1 Read data, Write steps, Keep direction, write timer data for delay, increment the file rd_idx,
      and test.
