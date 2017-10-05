# mapzenSketchesOF

### Two OpenFrameworks sketches showing how to load Mapzen terrain data and use it with fragment shaders for different interpretations.
#### The code and the shaders don't have many comments, but project files for _VS_ and _Xcode_ are included and no external Addons are needeed. You need to get your Mapzen Api-Key and replace the the #define in the mapZenHelper.h!

This sketch loads hightmap data (terrarium) and the normal-map data from mapzen
 via the mapZenHelper class. After loading the 256 sized chunks the polyMap class
 will be used to convert the image encoding to usefull images. The polyMap class
 also uses openCV to create hight-contours (set bei the lines amout). Then those
 textures are send to the fragment shader for different image manipulation.
 



 
 ---
 SketchA is using a 2D approach with different lightning conditions via the shader, you might need to shuffle the sliders a bit.
![Image of sketchA](https://c1.staticflickr.com/5/4320/36187433925_97602705b0_o.png)

SketchB is 3D with a vertext shader, but the code is more dirty and has some leftovers...
![Image of sketchB](https://c1.staticflickr.com/5/4297/36187434125_110e16d960_o.png)
