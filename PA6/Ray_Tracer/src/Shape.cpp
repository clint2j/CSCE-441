#include "Shape.h"

Shape::Shape(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 mirror, float shininess, glm::vec3 refract, float _nRefract)
	: ka(ambient), kd(diffuse), ks(specular), km(mirror), n(shininess), ior(_nRefract), kt(refract)
{
}

Shape::~Shape()
{
}

//MaterialRecord Shape::GetParameters()
//{
//	return MaterialRecord(ka, kd, ks, km, n);
//}

//float Shape::Intersect(Ray ray, float t0, float t1)
//{
//	return 0.0f;
//}

MaterialRecord::MaterialRecord(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 mirror, float shininess, glm::vec3 refract, float _nRefract, glm::vec3 _normal)
	: ka(ambient), kd(diffuse), ks(specular), km(mirror), n(shininess), normal(_normal), ior(_nRefract), kt(refract)
{
}

//MaterialRecord::MaterialRecord(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 mirror, float shininess, glm::vec3 normal, glm::vec3 position)
//	: ka(ambient), kd(diffuse), ks(specular), km(mirror), n(shininess), normalVector(normal), positionVector(position)
//{
//}
