#pragma once

namespace network
{
	class CNetwork;
}

class CGame
{
public:
	void Init(const WindowInfo& Info, std::shared_ptr<network::CNetwork> pNetwork);
	void Update();
};

