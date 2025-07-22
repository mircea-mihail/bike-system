# Bike System
This repo tracks my progress developing a three-module bike system for my Elops Speed 500 bike from Decathlon. It is able to warn the cyclist in real time of 5 important traffic signs using the bike camera module, display real time movement metrics using the magnetic sensor on the wheel via the bike calculator module and a third component can analyse data stored on a microSD by plotting graphs enabled by a python 3 script.
<p align="center">
	<img src="documents/github-images/high-level-diagram.png" alt="high-lvl-diagram" width="75%">
</p>

## The Bike Camera
The Bike Camera is able to detect 5 important street signs in real time: Pedestrian crossing, Give way, No bikes, Stop and Wrong way. The detections happen in real time: about 5 photos can be taken and processed every second.

<p align="center">
  <img src="documents/github-images/street-signs/crossing.png"   alt="crossing-sign"   height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/street-signs/give-way.png"   alt="give-way-sign"  height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/street-signs/no-bikes.png"   alt="no-bikes-sign"  height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/street-signs/stop.png"       alt="stop-sign"      height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/street-signs/wrong-way.png"  alt="wrong-way-sign" height="290">
</p>

<details>
<br>
<summary> <B>How signs are detected</B> </summary>
The main approach for detecting road signs involves finding regions of interest (ROI) upon which a template matching algorithm can be applied to check for the 5 road signs mentioned above. Because it is expensive to check the color of each individual pixel, binary masks are generated for each picture. They indicate the presence of the 4 main colors present in the templates used. Those colors are: bright red, dark red, white and black.

<br>
<p align="center">
  <img src="documents/github-images/masks-used/original.png"   	alt="orignial"   height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/masks-used/bright-red.png"  alt="bright-red-mask"  height="290">
</p>
<br>
<p align="center">	
  <img src="documents/github-images/masks-used/black.png"   	alt="black-mask"  height="290">
  &nbsp;&nbsp;
  <img src="documents/github-images/masks-used/white.png"       alt="white-mask"      height="290">
</p>

</details>
