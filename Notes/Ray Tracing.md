## Ray Casting
- Generate an image by casting one ray per pixel
### Pinhole Camera Model
1. Eye ray (starts at eye)
2. Finds closest scene intersection point
3. Performs shading calculation at point (eg. Phong Model)
4. For each light source, draw a ray from the point to each light source. If that ray is blocked, then don't use that light source (shadow)
## Recursive Ray Tracing Model
>Same as pinhole except at the first point, additionally reflection and refraction rays are sent out to find the effects of any reflected or refracted light
![[Pasted image 20241123152956.png]]

## Ray Calculations
### Ray Equation
$$r(t)=o+td \text{ for } 0\leq t<\infty$$
- This equation is found by putting o as the eye and  d is the unit direction found by subtracting the eye from each point on the image plane

### Intersection with Sphere
The sphere is defined as: $$(p-c)^2-R^2=0$$
so the intersection is found by solving for $$(o+td-c)^2-R^2=0$$
so I can use the quadratic formula where $a=d\cdot d$, $b=2(o-c)\cdot d$, and $c=(o-c)\cdot (o-c) - R^2$
### Intersection with a Plane
The plane is defined as:$$(p-p')\cdot N=0$$
where p is all points on the plane and p' is any random point and N is the unit normal vector to the plane
Thus the intersection point is found using:$$(o+td-p')\cdot N = 0$$Solving for t produces the plane intersection equation: $$t=\frac{(p'-o)\cdot N}{d\cdot N}$$
### Intersection with a triangle
1. Check where the ray and plane of the triangle intersect
2. Test if intersection point is inside triangle
### Intersection with a triangle mesh
- Simple, but slow version:
	- intersect ray with each triangle
- Faster
	- Moller Trumbore Algorithm
## Ray Tracing Details
### Important implementation details
#### 1. Shadow Acne
- If the shadow rays start at t=0, due to floating point errors they may intersect with the intersecting object, giving an errant shadow. Thus start at $t=\epsilon$ 