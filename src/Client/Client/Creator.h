#pragma once

class CGameObject;
class Texture;
class Shader;

class Creator
{
public:
	static shared_ptr<CGameObject> CreateUIObject(shared_ptr<Texture> texture, shared_ptr<Shader> shader);
	static shared_ptr<CGameObject> CreateUIObject(shared_ptr<Texture> texture, shared_ptr<Shader> shader, bool visible);
	static shared_ptr<CGameObject> CreatePopUpObject(shared_ptr<Texture> texture, shared_ptr<Shader> shader, bool visible, bool blur = false);

};

