## Image Generation Example
![[Pasted image 20240919231326.png]]

## Camera Space
### Pose
#### Origin (eye)
#### look at (center)
#### up
![[Pasted image 20240923234655.png]]

## View Transform
1. Construct camera coordinate frame
2. Represent all vertices in this camera coordinate system
>A coordinate frame is any set of 3 vectors such that the following holds true
>$$||\vec{u}||=||\vec{v}||=||\vec{w}||=1$$
>$$\vec{u}\cdot \vec{v} = \vec{v}\cdot \vec{w} = \vec{w}\cdot \vec{u} = 0$$
>$$\vec{w} = \vec{u} \times \vec{v}$$
>
### Constructing the camera coordinate fram
![[Pasted image 20240923235541.png]]
#### Also the following is true: $\vec{v} = \vec{w} \times \vec{u}$
## Projection transform
Creates a projection matrix which transforms the 3d coordinates into 2d coordinates while simulating the perspective effect where objects farther away appear smaller.