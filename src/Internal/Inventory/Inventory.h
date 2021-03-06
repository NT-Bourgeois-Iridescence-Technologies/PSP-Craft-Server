#pragma once
#include "Slot.h"

namespace Minecraft::Server::Internal::Inventory {
	class Inventory {
	public:
		Inventory();
		~Inventory();

		void init();
		void cleanup();

		Slot* getSlot(uint8_t idx);

		void update();

		bool addItem(Slot slt);

	private:
		Slot inventorySpots[46];
	};
}