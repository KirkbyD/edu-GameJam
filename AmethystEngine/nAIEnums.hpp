#pragma once
namespace nAIEnums {
	class AIEnums {
	public:
		enum class eAIBehviours : size_t {
			IDLE = 1, SEEK = 2, APPROACH = 4, PURSURE = 8, FLEE = 16, EVADE = 32, WANDER = 64, FOLLOW_PATH = 128, SEPERATION = 256, ALIGNMENT = 512, COHESION = 1024,
			FLOCK = SEPERATION | ALIGNMENT | COHESION, FLOCK_PATH = FLOCK | FOLLOW_PATH
		};
		enum class eFormationBehaviours { NONE, CIRCLE, SQUARE, WEDGE, LINE, COLUMN };
		enum class ePlayerTypes { PLAYER, FRIENDLY, NEUTRAL, UNFRIENDLY };

		template <typename EnumClass, EnumClass EnumVal>
		static void ToString_impl();

		template<auto EnumVal>
		static void ToString() { AIEnums::ToString_impl<decltype(EnumVal), EnumVal>(); }
	};

	template<typename EnumClass, EnumClass EnumVal>
	void AIEnums::ToString_impl() {
		if (std::is_same_v<EnumClass, nAIEnums::AIEnums::eAIBehviours>) {
			switch ((nAIEnums::AIEnums::eAIBehviours)EnumVal) {
			case nAIEnums::AIEnums::eAIBehviours::ALIGNMENT:
				std::cout << "ALIGNMENT" << std::endl;
				break;

			default:
				break;
			}
			return;
		}

		if (std::is_same_v<EnumClass, nAIEnums::AIEnums::ePlayerTypes>) {
			switch ((nAIEnums::AIEnums::ePlayerTypes)EnumVal) {
			case nAIEnums::AIEnums::ePlayerTypes::PLAYER:
				std::cout << "PLAYER" << std::endl;
				break;

			default:
				break;
			}
			return;
		}

		if (std::is_same_v<EnumClass, nAIEnums::AIEnums::eFormationBehaviours>) {
			switch ((nAIEnums::AIEnums::eFormationBehaviours)EnumVal) {
			case nAIEnums::AIEnums::eFormationBehaviours::CIRCLE:
				std::cout << "CIRCLE" << std::endl;
				break;

			default:
				break;
			}
			return;
		}
	}
}
