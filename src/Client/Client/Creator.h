#pragma once

class CGameObject;
class Texture;
class Shader;

class Creator
{
public:
	static shared_ptr<CGameObject> CreateUIObject(shared_ptr<Texture> texture, shared_ptr<Shader> shader);

};

