>Functions acting on points

## Linear 2d
### Represented by 2x2 matrix multiplication (linear operator)
#### Rotation
$$R(\theta) = \begin{pmatrix}
\cos\theta & -\sin\theta \\
\sin\theta & \cos\theta
\end{pmatrix}$$
#### Reflection
$$R(\theta) = \begin{pmatrix}
-1 & 0 \\
0 & 1
\end{pmatrix}$$
#### Scale
$$R(\theta) = \begin{pmatrix}
s_x & 0 \\
0 & s_y
\end{pmatrix}$$
#### Shear
$$R(\theta) = \begin{pmatrix}
1 & a \\
0 & 1
\end{pmatrix}$$
## Homogeneous Coordinates
- Represent an n-dimensional euclidean point using n+1 point (add a dimension)
- Allows some non linear operations to be performed on points with matrix multiplication
- For example:
#### Translation
$$\begin{pmatrix}
1 & 0 & t \\
0 & 1 & 0 \\
0 & 0 & 1
\end{pmatrix}
\begin{pmatrix}
x \\
y \\
1
\end{pmatrix}
=
\begin{pmatrix}
x + t \\
y \\
1
\end{pmatrix}$$
* To convert from homogeneous to euclidean, divide the entire point by the last coordinate and truncate the last value
* thus:

## Overall 2D Transformation
#### Scale

$$
S(s_x, s_y) = \begin{pmatrix}
s_x & 0 & 0 \\
0 & s_y & 0 \\
0 & 0 & 1
\end{pmatrix}
$$

#### Rotation

$$
R(\alpha) = \begin{pmatrix}
\cos \alpha & -\sin \alpha & 0 \\
\sin \alpha & \cos \alpha & 0 \\
0 & 0 & 1
\end{pmatrix}
$$

#### Translation

$$
T(t_x, t_y) = \begin{pmatrix}
1 & 0 & t_x \\
0 & 1 & t_y \\
0 & 0 & 1
\end{pmatrix}
$$
### Composite Transformations
- Multiply matrices to get a composite transformation
$$A_n(...A_2(A_1(x)))=A_n\cdot ... \cdot A_2 \cdot A_1 \cdot \begin{pmatrix}
x \\
y \\
1
\end{pmatrix}$$
