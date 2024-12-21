Notes:
I implemented antialiasing, refraction, and soft shadows.

AntiAliasing:
It is on by default. Push 'A' to increase the radius of the random rays to increase its effect and 'a' to decrease its effect. It currently fires 4 rays with a slight random offset from each pixel and averages their colors.

Soft Shadows:
It is on by default firing 5 rays per shadow calculation. The 'S' operation increases the randomness radius of the shadow rays and 's' decreases the radius.

Refraction:
It is off by default and you can turn it on in the bottom left sphere by clicking 'R' and you can turn it off by clicking 'r'. 

Note rendering in release mode should take between 8-20 seconds so make one command and wait for it to finish before inputing another command (you can look at the terminal to see if the rerendering has happened yet)