#include "Emitter.h"
#include "Debug.h"
#include <algorithm>

Emitter::Emitter(Vector3 position, int numberOfParticles, std::shared_ptr<Geometry> geometry,
	std::shared_ptr<Material> material, ID3D11ShaderResourceView* textureRV)
{
	GameObject* gameObject;

	//scale the velocities according to their list in the array
	int IndexScale; 

	float spawnTimer;
	float randomSideForce;

	//Randomness for the Z axies
	float randomZForce;
	float overallZForce;

	//Randomness for the X axies
	float randomXForce;
	float overallXForce;

	for (int i = 0; i < numberOfParticles; i++)
	{
		#pragma region Randomise Values
		IndexScale = i;
		IndexScale = clamp(IndexScale, 5, numberOfParticles);

		spawnTimer = (rand() % 15) / 5;
		randomSideForce = static_cast<float>(std::rand()) / RAND_MAX * 2.0 - 1.0;

		randomXForce = rand() % 100;
		randomXForce = clamp(randomXForce, 20.0f, 30.0f);
		overallXForce = randomXForce * randomSideForce;

		randomZForce = rand() % 100;
		randomZForce = clamp(randomZForce, 20.0f, 30.0f);
		overallZForce = randomZForce * randomSideForce;
		#pragma endregion //Randomise Values

		gameObject = new GameObject("Donut", geometry, material);
		gameObject->SetPhysicsModel(new ParticleModel(gameObject->GetTransform(), spawnTimer, position, false));
		gameObject->GetTransform()->SetScale(1, 1, 1);
		gameObject->GetApperance()->SetTextureRV(textureRV);
		gameObject->GetPhyicsModel()->AddRelativeForce(Vector3(overallXForce * IndexScale, 20 * IndexScale, overallZForce * IndexScale), Vector3());

		_particles.push_back(gameObject);
	}
}

void Emitter::Update(float deltaTime)
{
	for (int i = 0 ; i < _particles.size(); i++)
	{
		_particles[i]->Update(deltaTime);
	}
}

void Emitter::Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer& cb)
{
	// Render all scene objects
	for (auto gameObject : _particles)
	{
		// Get render material
		std::shared_ptr<Material> material = gameObject->GetApperance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetWorldMatrix());

		// Set texture
		if (gameObject->GetApperance()->HasTexture())
		{
			ID3D11ShaderResourceView* textureRV = gameObject->GetApperance()->GetTextureRV();
			immediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->GetApperance()->Draw(immediateContext);
	}

}
