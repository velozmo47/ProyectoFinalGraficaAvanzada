class Collectable
{
public:
	Model* model;
	glm::vec3 posicion;
	glm::mat4 modelMatrixCollider;
	AbstractModel::OBB collider;
	bool collected;


	Collectable(Model* model, glm::vec3 posicion) : model(model)
	{
		this->model = model;
		this->posicion = posicion;

		collected = false;

		modelMatrixCollider = glm::mat4(1.0);
		modelMatrixCollider[3] = glm::vec4(posicion, 1);
	}

	AbstractModel::OBB& GetCollider()
	{
		collider.u = glm::quat_cast(modelMatrixCollider);
		modelMatrixCollider = glm::translate(modelMatrixCollider, model->getObb().c);
		collider.c = glm::vec3(modelMatrixCollider[3]);
		collider.e = model->getObb().e * glm::vec3(1.0);
		return collider;
	}

	void Effect()
	{
		modelMatrixCollider = glm::rotate(modelMatrixCollider, glm::radians(1.0f), glm::vec3(0, 1, 0));
	}
};
