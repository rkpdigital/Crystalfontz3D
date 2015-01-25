//Search for c
// VISUALIZATION SECTION
//and
// BUILD SECTION 
//to get to the good stuff!
//
// Load in OpenScad and press F4 to see what is going on.
//
//=======================================================================================
//
// Use http://notepad-plus-plus.org/ to edit, set language to "JavaScript"
//
// Use http://www.openscad.org/ to render.
//  Set View -> Hide Editor
//  Set Design -> Automatic Reload and Compile
// then when you save in Notepad++ OpenSCAD will refresh.
//
// Set one of the the conditionals in the BUILD SECTION, then
// use F6 in OpenSCAD (wait for it) then use Design -> Export as STL
// to get STL parts.
//
//=======================================================================================
//Disable $fn and $fa
$fn=0;
$fa=0.01;

//Use only $fs
//$fs=0.15;
$fs=0.2;

//=======================================================================================
ftrue=false;
//=======================================================================================
//rotate axes
X=[1,0,0];
Y=[0,1,0];
Z=[0,0,1];

//=======================================================================================
module radius(radius,length)
{
	linear_extrude(height = length, center = false, convexity = 4, twist = 0)
	difference()
	{
		translate([-0.1,-0.1]) square([radius+0.1,radius+0.1]);
		translate([radius,radius]) circle(r=radius);
	}
}
//=======================================================================================
module capsule(length,radius)
{	cylinder(r=radius,h=length-radius,center=true);
	translate([0,0,-(length-radius)/2]) sphere(r=radius);
	translate([0,0,+(length-radius)/2]) sphere(r=radius);
}
//=======================================================================================  
module right_triangle(side1,side2,height)
  {
  translate([0,0,-height/2]) linear_extrude(height=height) polygon(points=[[0,0],[0,side2],[side1,0]],paths=[[0,1,2]]);
  }
//=======================================================================================  
module isosceles_triangle(base,length,height)
  {
  rotate(90,[1,0,0]) linear_extrude(height=length) polygon(points=[[-base/2,0],[+base/2,0],[0,height]],paths=[[0,1,2]]);
  }
//=======================================================================================  
module oval_extrusion(x_length,y_thickness,z_height)
  {
  linear_extrude(height = z_height, center = true, convexity = 2, twist = 0)
    union()
      {
      square([x_length-y_thickness,y_thickness],center=true);
      translate([+(x_length-y_thickness)/2,0,0]) circle(r=y_thickness/2,center=true);
      translate([-(x_length-y_thickness)/2,0,0]) circle(r=y_thickness/2,center=true);
      }
  }
//=======================================================================================  
module bolt_3mm_button_socket_head_clearance(length)
  {
  //bolt - just 3mm but 0.5mm longer
  translate([0,0,-0.1]) cylinder(r=3/2,h=length+0.1+0.5);
  //bolt's head add 0.5mm radius
  translate([0,0,-5])cylinder(r=5.7/2+0.5,h=5);
  }
//=======================================================================================  
module bolt_3mm_button_socket_head(length)
  {
  //bolt Reduce by 0.2mm to act like threads
  //and to not hit clearance
  translate([0,0,-0.1]) cylinder(r=3/2-0.1,h=length+0.1);
  difference()
    {
    //bolt's head
    intersection()
	  {
	  //cylinder
      translate([0,0,-1.65])cylinder(r=5.7/2,h=1.65);
	  //sphere
	  translate([0,0,1.1]) sphere(r=3.2);
      }
    //bolt's hex socket
    translate([0,0,-1.75]) cylinder(r=(2/2)/sin(60),h=1.05,$fn=6);
    }
  }
//=======================================================================================  
module bolt_3mm_socket_head_bolt(length)
  {
  difference()
    {
    union()
      {
      //bolt
      translate([0,0,-0.1]) cylinder(r=3/2,h=length+0.1);
      //bolt's head
      translate([0,0,-3]) cylinder(r=5.5/2,h=3,$fn=18);
      }
    //bolt's hex socket
    translate([0,0,-3.1]) cylinder(r=(2.5/2)/sin(60),h=1.6,$fn=6);
    }
  }
//=======================================================================================  
module nut_3mm_x_5p5_x_2p5_nylock_clearance()
  {
  //hex
  translate([0,0,1.25+2])
    cylinder(r=(5.7/2)/sin(60),h=4+3,center=true,$fn=6);
  }
//=======================================================================================  
module nut_3mm_x_5p5_x_2p5_nylock()
  {
  difference()
    {
	union()
	  {
	  //hex
      cylinder(r=(5.5/2)/sin(60),h=2.5,center=true,$fn=6);
      //round bit
	  translate([0,0,1.5]) cylinder(r=(5/2),h=2.5,center=true);
      }
	//hole
	translate([0,0,0.75]) cylinder(r=3/2,h=4+0.2,center=true);
    }
  }
//=======================================================================================  
module nut_3mm_x_5p5_x_2p5()
  {
  difference()
    {
    cylinder(r=(5.5/2)/sin(60),h=2.5,center=true,$fn=6);
    cylinder(r=3/2,h=2.5+0.2,center=true);
    }
  }
//=======================================================================================  
module washer_3mm_0p47_thick()
  {
  difference()
    {
    cylinder(r=6.76/2,h=0.47,center=true);
    cylinder(r=3.25/2,h=0.47+0.2,center=true);
    }
  }
//=======================================================================================  
XC_PCB=(-121.5/2);
YC_PCB=(-77.5/2);
//---------------------------------------------------------------------------------------
module CFA921_PCB()
  {
  //PCB
  translate([XC_PCB,YC_PCB,0])cube([121.5,77.5,1.6]);
  //ETH
  translate([XC_PCB,-18-15.5/2,(-11.35+8.85)+1.6])cube([21.5,15.5,11.35]);
  //USB
  translate([XC_PCB,16-9,0])cube([21.5,18,8]);
  //power 
  translate([XC_PCB,0,1.25+1.6])rotate(90,Y) cylinder(r=4/2,h=10);
  //debug 
  translate([54.25-5/2,0-38.1/2,1.6])cube([5,38.1,10]);
  }
//=======================================================================================  
XC_LCD=(-120.8/2);
YC_LCD=(-75.85/2);
//---------------------------------------------------------------------------------------
module CFA921_LCD()
  {
  //Overall
  translate([XC_LCD,YC_LCD,0])cube([120.8,75.85,4.3]);
  //TS
  color([0,1,0,0.4])
    translate([-110.8/2,YC_LCD+2.78,-0.01])cube([110.8,66.22,0.01]);
  //AA
  color([1,0,0,0.4])
    translate([-108/2,YC_LCD+3.3,-0.02])cube([108,64.8,0.01]);
  }
//=======================================================================================  
module bezel_cross_section(thick,wide,radius,thin,slope)
  {
  polygon(points=[
   [0,0],
   [0,thin],
   [slope,thick],
   [wide-radius,thick],
   [wide-radius,thick-radius],
   [wide,thick-radius],
   [wide,0]],
   paths=[[0,1,2,3,4,5,6]]);
 
 difference()
   {
   //circle
   translate([wide-radius,thick-radius,]) circle(r=radius);
   //trim bottom
   translate([wide-radius*2,thick-radius*2-0.01]) square([radius*2+0.02,radius]);
   //trim left
   translate([wide-radius*2-0.01,thick-radius*2]) square([radius,radius*2+0.02]);
   }
  } 
//======================================================================================= 
module bezel_corner(thick,wide,radius,thin,slope)
  {
  intersection()
    {
    rotate_extrude() bezel_cross_section(thick,wide,radius,thin,slope);
    cube([wide+1,wide+1,thick+1]);
    }
  }
//======================================================================================= 
module Case_Bezel(w,h)
  {
  //longer along thick edge 
  rotate(-90,X) rotate(-90,Z) translate([-0.1,0,0]) linear_extrude(height=w+7.5+0.2)
    bezel_cross_section(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);

  //"double" along thick edge 
  translate([0,w,0]) rotate(-90,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  translate([0,w+7.5/2,0]) rotate(-90,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  translate([0,w+7.5,0]) rotate(-90,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
	
  translate([0,0,-h]) rotate(-90,Z) translate([0,0,0]) linear_extrude(height=h+0.2)
    bezel_cross_section(thick=4,wide=13,radius=2,thin=0.75,slope=5);
 
  //wider along thick edge 
  #translate([0,w,0]) rotate(180,X) rotate(-90,Z) translate([0,0,0]) linear_extrude(height=h+0.2)
    bezel_cross_section(thick=4,wide=13+7.5,radius=2,thin=0.75,slope=5);
	

  //"double" along thick edge 
  translate([0,w,-h]) rotate(180,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  translate([0,w+7.5/2,-h]) rotate(180,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  translate([0,w+7.5,-h]) rotate(180,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
	
  translate([0,0,-h]) rotate(90,X) rotate(180,Z) rotate(-90,Y)
    bezel_corner(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  //longer along thick edge 
  translate([0,w+7.5,-h]) rotate(90,X) rotate(-90,Z) translate([-0.1,0,0]) linear_extrude(height=w+7.5+0.2)
    bezel_cross_section(thick=4,wide=13,radius=2,thin=0.75,slope=5);
  }
//======================================================================================= 
module bolt_stack(length)
  {
  bolt_3mm_button_socket_head(length=length);
  //bolt_3mm_button_socket_head_clearance(length=length);
  translate([0,0,length-3]) nut_3mm_x_5p5_x_2p5_nylock();
  //translate([0,0,length-3]) %nut_3mm_x_5p5_x_2p5_nylock_clearance();
  }
//======================================================================================= 
module bolt_stacks(length)
  {
  translate([-50, 36+1,-9]) bolt_stack(length=length);
  translate([-25, 36+1,-9]) bolt_stack(length=length);
  translate([  0, 36+1,-9]) bolt_stack(length=length);
  translate([ 25, 36+1,-9]) bolt_stack(length=length);
  translate([ 50, 36+1,-9]) bolt_stack(length=length);
  
  translate([-50,-36-1,-9]) bolt_stack(length=length);
  translate([-25,-36-1,-9]) bolt_stack(length=length);
  translate([  0,-36-1,-9]) bolt_stack(length=length);
  translate([ 25,-36-1,-9]) bolt_stack(length=length);
  translate([ 50,-36-1,-9]) bolt_stack(length=length);
  
  translate([-58.5-1,-24,-9]) rotate(-90,Z) bolt_stack(length=length);
  translate([-58.5-1, -5,-9]) rotate(-90,Z) bolt_stack(length=length);
  translate([-58.5-1, 16,-9]) rotate(-90,Z) bolt_stack(length=length);

  translate([+58.5+1,-20,-9]) rotate(-90,Z) bolt_stack(length=length);
  translate([+58.5+1, -0,-9]) rotate(-90,Z) bolt_stack(length=length);
  translate([+58.5+1, 20,-9]) rotate(-90,Z) bolt_stack(length=length);
  }
//======================================================================================= 
module bolt_clearance(length)
  {
  bolt_3mm_button_socket_head_clearance(length=length);
  translate([0,0,length-3]) nut_3mm_x_5p5_x_2p5_nylock_clearance();
  }
//======================================================================================= 
module bolt_clearances(length)
  {
  translate([-50, 36+1,-9]) bolt_clearance(length=length);
  translate([-25, 36+1,-9]) bolt_clearance(length=length);
  translate([  0, 36+1,-9]) bolt_clearance(length=length);
  translate([ 25, 36+1,-9]) bolt_clearance(length=length);
  translate([ 50, 36+1,-9]) bolt_clearance(length=length);
  
  translate([-50,-36-1,-9]) bolt_clearance(length=length);
  translate([-25,-36-1,-9]) bolt_clearance(length=length);
  translate([  0,-36-1,-9]) bolt_clearance(length=length);
  translate([ 25,-36-1,-9]) bolt_clearance(length=length);
  translate([ 50,-36-1,-9]) bolt_clearance(length=length);
  
  translate([-58.5-1,-24,-9]) rotate(-90,Z) bolt_clearance(length=length);
  translate([-58.5-1, -5,-9]) rotate(-90,Z) bolt_clearance(length=length);
  translate([-58.5-1, 16,-9]) rotate(-90,Z) bolt_clearance(length=length);

  translate([+58.5+1,-20,-9]) rotate(-90,Z) bolt_clearance(length=length);
  translate([+58.5+1, -0,-9]) rotate(-90,Z) bolt_clearance(length=length);
  translate([+58.5+1, 20,-9]) rotate(-90,Z) bolt_clearance(length=length);
  }
//======================================================================================= 
module Case_Top_Sides(w,h,d,r)
  {
  //translate([-w/2,-h/2,-d/2]) cube([w,h,d]);

  difference()
    {
	union()
	  {
      translate([-w/2,-h/2-r,-3-4.3-2]) cube([w,h+r+r,d]);
      translate([-w/2-r,-h/2,-3-4.3-2]) cube([w+r+r,h,d]);
      
      //vert edges
	  translate([ w/2, h/2,-3-4.3]) cylinder(r=r,h=d-2);
      translate([ w/2,-h/2,-3-4.3]) cylinder(r=r,h=d-2);
	  translate([-w/2, h/2,-3-4.3]) cylinder(r=r,h=d-2);
      translate([-w/2,-h/2,-3-4.3]) cylinder(r=r,h=d-2);
	  //corners
	  translate([-w/2,-h/2,-3-4.3-2-0.0]) cylinder(r1=0.72,r2=r,h=2.0);
      translate([-w/2, h/2,-3-4.3-2-0.0]) cylinder(r1=0.72,r2=r,h=2.0);
      translate([ w/2,-h/2,-3-4.3-2-0.0]) cylinder(r1=0.72,r2=r,h=2.0);
      translate([ w/2, h/2,-3-4.3-2-0.0]) cylinder(r1=0.72,r2=r,h=2.0);
	  
	  
	  
      }
    //Cut-out for PCB & LCD
    translate([XC_PCB-0.249,YC_PCB-0.249,-3-4.3])cube([121.5+0.498,77.5+0.498,21]);	
    //Cut out for AA
    translate([0,5.4,0])
      {  
      //main hole
      translate([-108/2-0.25,YC_LCD+3.3-1.1,-10])cube([108+0.5,64.8+0.5,5]);
	  //angles::inner corners
	  translate([-108/2-0.25,YC_LCD+3.3-1.1,-10.29])  cylinder(r1=2,r2=0.1,h=3);
      translate([-(-108/2-0.25),YC_LCD+3.3-1.1,-10.29])  cylinder(r1=2,r2=0.1,h=3);
      translate([-108/2-0.25,-YC_LCD-3.3-1.1-3.95,-10.29])  cylinder(r1=2,r2=0.1,h=3);
      translate([-(-108/2-0.25),-YC_LCD-3.3-1.1-3.95,-10.29])  cylinder(r1=2,r2=0.1,h=3);
	
      //angles::inner edges
	  translate([-108/2-0.25,YC_LCD+3.3-1.1 -1.6,-9.65]) rotate(-32.5,X) cube([108+0.5,5,5]);
	  translate([-108/2-0.25,-6.15-(YC_LCD+3.3-1.1 -1.6),-9.65]) rotate(90+32.5,X) cube([108+0.5,5,5]);
      translate([-108/2-0.25 -1.822,YC_LCD+3.3-1.1,-10])rotate(32.5,Y) cube([5,64.8+0.5,5]);
      translate([-(-108/2-0.25) -4.42,YC_LCD+3.3-1.1,-9.5])rotate(90-32.5,Y) cube([5,64.8+0.5,5]);
	  }
		
    //angles::outer edges	
    translate([w/2-0.686,-h/2-2.5,-11.515])rotate(32.5,Y) cube([5,h+5+0.5,5]);
    translate([-w/2-6.217,-h/2-2.5,-9.986])rotate(90-32.5,Y) cube([5,h+5+0.5,5]);
	translate([-w/2-2.5,h/2-0.686,-11.515]) rotate(-32.5,X) cube([w+5,5,5]);
	translate([-w/2-2.5,-h/2+0.686,-11.515]) rotate(90+32.5,X) cube([w+5,5,5]);
	
    //Side holes
    //ETH
    translate([-54.15-14,-18-15.5/2-1+0.8,(-11.35+8.85)+1.6-1])cube([21.5+6,15.5+0.7,11.75]);
    //USB
    translate([-54.15-14,16-9-.5,0-1+0.75])cube([21.5+6,18+1,8+2]);
    //power 
    translate([-54.15-9.25,0,1.25+1])rotate(90,Y) cylinder(r=9/2,h=1);	
    translate([-54.15-10,0,1.25+1])rotate(90,Y) cylinder(r=4.25/2,h=5);	
    
    //USB Ethernet
    translate([-(XC_PCB + 26.5), -(h/2 + 3), d - 11.9 ])rotate(180, Y)cube([13, 3, 6.4]);
   #translate([-(XC_PCB + 25), -(h/2 + 3), d - 11.9 ])rotate(90, X) cylinder(r=1,h=5);	
    
    //SD Card
    translate([XC_PCB + 56, -(h/2 + 3), d - 11.9 ])rotate(180, Y)cube([13, 4, 4]);
    
    }
  }
//======================================================================================= 
module Case_Bottom(w,h,d,r)
  {
  difference()
    {
	  union()
	    {
      translate([-54.25, 33,2.6]) cylinder(r=4.5,h=9);
      translate([ 54.25, 33,2.6]) cylinder(r=4.5,h=9);
      translate([-54.25,-34,2.6]) cylinder(r=4.5,h=9);
      translate([ 54.25,-34,2.6]) cylinder(r=4.5,h=9);
	
      translate([XC_PCB-0.125,YC_PCB-0.125,8.8])cube([121.5+0.25,7,1]);	
      translate([XC_PCB-0.125,77.5-2.25+YC_PCB-0.125,6.8])cube([121.5+0.25,2.5,3]);	
	
      translate([XC_PCB-0.125,YC_PCB-0.125,9.7])cube([121.5+0.25,77.5+0.25,3]);	  
      }
	  
    //ETH
    translate([XC_PCB-1,-18-15.5/2-1,(-11.35+8.85)+1.6])cube([23.5,17.5,12]);
    //debug 
    translate([54.25-7/2,0-40.1/2,2])cube([7,40.1,10]);

    //heat spreader
    translate([-(XC_PCB + 19.64), YC_PCB + 5.5, 14])rotate(-180,Y)cube([62, 33, 6]);
    //Serial port
    translate([-(XC_PCB + 83.5), -1.5, 14])rotate(-180,X)cube([3, 13, 6]);

    translate([-54.25, 33,2.5]) cylinder(r=2.2,h=4);
    translate([ 54.25, 33,2.5]) cylinder(r=2.2,h=4);
    translate([-54.25,-34,2.5]) cylinder(r=2.2,h=4);
    translate([ 54.25,-34,2.5]) cylinder(r=2.2,h=4);
    }
  }
//======================================================================================= 
// VISUALIZATION SECTION
if(ftrue)
  {
  }
//=======================================================================================  
// BUILD SECTION 
//---------------------------------------------------------------------------------------
if(true)
  {
  //build platform for debug
  //color([0.5,0.5,0.5,0.1]) translate([-50,-50,-1.01]) %cube([100,100,1]);

  /*
  difference()
    {
    color([0.5,0.5,1,0.6])
	  translate([-54.15+0.2+3.1-0.5,-31.5+0.02+3.1-0.5,-3-3.97])
	    rotate(-90,Z)
		  rotate(90,Y)
		    Case_Bezel(w=94.2+1,h=56.76+1);
	bolt_clearances(length=22);
	}
*/

//  color([0.7,0.7,0.7,0.8]) bolt_stacks(length=23);
  //color([0.0,0.0,1.0,0.4])CFA921_PCB();  
  //translate([0,-0.825,-3-4.3]) CFA921_LCD();  //3mm tape, 4.3 mm LCD

   //color([1,0.5,0.5,0.2])
     Case_Top_Sides(w=121.5+0.5,h=77.5+0.5,d=18+2+2,r=2);
   //color([0.25,0.55,1,0.2])
   
   
   
   //  Case_Bottom(w=121.5+0.5,h=77.5+0.5,d=18+2+2,r=2);
  
  /*
  
  translate([0,32,-3])bolt_3mm_button_socket_head(length=10);
  translate([0,32,-3])%bolt_3mm_button_socket_head_clearance(length=10);
  translate([0,32,3.5]) nut_3mm_x_5p5_x_2p5_nylock();
  translate([0,32,3.5]) %nut_3mm_x_5p5_x_2p5_nylock_clearance();

*/
  }
//=======================================================================================  
