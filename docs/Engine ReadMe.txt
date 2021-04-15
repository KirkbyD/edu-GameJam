AmethystEngine by:
	Dylan Kirkby,
	Brian Cowan,
	Ivan Parkhomenko

Created in Visual Studio Community 2019
Runs in x64 only, Release recommended for performance.

External files located in \AmethystEngine\data
Configuration files located in \AmethystEngine\data\config

Controls:
Mouse:
	Move		Adjust camera viewing angle
		Complex type selected
			Also Rotate objects Yaw/Pitch
Keyboard:
	`		Toggle Developer Console (Freeze physics and audio system updates)
	[		Select Model Objects
	]		Select Light Objects
	\		Select Complex Objects
	=		Select Audio Objects
	P		Use Free Flying Camera
	Left Cursor	Move backwards through currently selected object type
	Right Cursor	Move forwards through currently selected object type


	Normal Controls
	ESC		Exit Program
	F		Turn off currently selected light (no matter the object type selected)
	
	Model || Complex type selected
	A		Increase left velocity relative to camera
	D		Increase right velocity relative to camera
	W		Increase forward velocity relative to camera
	S		Increase backward velocity relative to camera
	SPACE		Reduce velocity 50% (hold to stop)	
	
	Complex Only
	Q		Roll 1 degree counter-clockwise
	E		Roll 1 degree clockwise
	
	Audio type selected (Channel groups)
	1		Toggle ECHO dsp on channel group 1
	2		Toggle TREMOLO dsp on channel group 1
	3		Toggle DISTORTION dsp on channel group 1
	4		Toggle ITECHO dsp on channel group 2
	5		Toggle FLANGE dsp on channel group 2
	6		Toggle NORMALIZE dsp on channel group 2
	7		Toggle HIGHPASS dsp on channel group 3
	8		Toggle ITLOWPASS dsp on channel group 3
	9		Toggle PITCHSHIFT dsp on channel group 3	
	KEYPAD 0	Toggle Pause
	KEYPAD 1	Raise Volume (max 100)
	KEYPAD 2	Lower Volume (min 0)
	KEYPAD 4	Raise Pitch (max 10)
	KEYPAD 5	Lower Pitch (min 0)
	KEYPAD 7	Pan Left
	KEYPAD 8	Pan Right
	

	Developer Controls
	ESC		Exit Developer Console if open
	ENTER		Send Command
	BACKSPACE	Erase last character
	.		Increase object movespeed (including freecam)
	,		Decrease object movespeed (including freecam)
	
	Model || Lights || Complex type selected
	CONTROL:	
		A		Move object left relative to camera
		D		Move object right relative to camera
		W		Move object forward relative to camera
		S		Move object backward relative to camera
		Q		Move object up relative to camera
		E		Move object down relative to camera
		
	Lights type only
	CONTROL:
		1		Increase Constant Attentuation
		2		Decrease Constant Attentuation
		3		Increase Linear Attentuation
		4		Decrease Linear Attentuation
		5		Increase Quadratic Attentuation
		6		Decrease Quadratic Attentuation
		V		Decrease Cone Inner Angle
		B		Increase Cone Inner Angle
		N		Decrease Cone Outer Angle
		M		Increase Cone Outer Angle
	
	COMMANDS
	Subsystems: model, light
	
	wireframe
	usage: wireframe "cmd" ?"Object_Name"
		enable		Turn on wireframe for current object
		disable		Turn off wireframe for current object
		toggle		Toggles wireframe for current object
	
	wireframeall
	usage: wireframe "cmd"
		enable		Turn on wireframe for all objects in current subsystem
		disable		Turn off wireframe for all objects in current subsystem
		toggle		Toggles wireframe for all objects in current subsystem
	
	visible
	usage: visible "Subsystem.cmd" ?"Target"	Targets given subsystem.
	usage: visible "cmd" ?"Target"				Targets active subsystem
		enable		Turn on visibility for object
		disable		Turn off visibility for object
		toggle		toggle visibility for object
	
	visibleall
	usage: visible "Subsystem.cmd"		Targets given subsystem.
	usage: visible "cmd" ?"Target"		Targets active subsystem
		enable		Turn on visibility for all objects in current subsystem
		disable		Turn off visibility for all objects in current subsystem
		toggle		toggle visibility for all objects in current subsystem
	
	savefiles		Saves current state to config files
	
	data
		show		Show debug information for current object in console.
		hide		Hides data display
		toggle		Toggles data display
	
	debug
		show		Enables debug information
		hide		disables debug infrmation
		toggle		toggles debug information
		model		Toggles model debugging and places a point at current model's center
		modelnormal	Toggles showing of current model's normals
		modeltriangle	Toggles showing of current model's triangles
					-Debug Renderer not handling triangles yet
		aabb		Toggles aabb debugging, show min and max corner.
		aabbnormal	Toggles showing of normals within aabb
		aabbtriangle	Toggles showing of triangles within aabb
					-Debug Renderer not handling triangles yet
		collision	Toggles showing of debug points, and placing 5 second collision dots.
		lights		Toggles showing of a white point at each light position, and attenuation spheres on current light.
					-Debug Renderer not handling spheres yet

	"Subsystem.control"	Swaps to controlling selected subsystem.
					-May not handled empty subsystem properly
	
	Create
	usage: Create model "mesh" "name"
	usage: Create light "name"
		Creates default object type with a given name (and mesh for model)
