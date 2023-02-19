#pragma once

class CGame
{
public:
	void Init(const WindowInfo& Info, std::shared_ptr<network::CNetwork> pNetwork);
	void Update();
};

