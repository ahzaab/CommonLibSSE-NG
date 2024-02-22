#include "RE/B/BGSDefaultObjectManager.h"

using namespace REL;

namespace RE
{
	namespace
	{
		constexpr auto kInvalid = (std::numeric_limits<std::size_t>::max)();

		inline std::size_t MapIndex(std::underlying_type_t<DefaultObjectID> a_idx) noexcept
		{
			if (a_idx <= stl::to_underlying(DefaultObjectID::kKeywordActivatorFurnitureNoPlayer)) {
				return a_idx;
			}

			// We split up like this:
			// VR-AE-SE, where each place occupies 10 bits

			std::size_t result;
			if SKYRIM_REL_CONSTEXPR (Module::IsVR()) {
				result = (0x3FF00000 & a_idx) >> 20;
				if (result >= DefaultObject::kTotalVR) {
					return kInvalid;
				}
			} else if SKYRIM_REL_CONSTEXPR (Module::IsAE()) {
				if (0x000FFC00 & a_idx) {
					result = (0x000FFC00 & a_idx) >> 10;
				} else {
					result = (0x00003FF & a_idx);
				}
				if (result >= DefaultObject::kTotalAE) {
					return kInvalid;
				}
			} else {
				result = (0x00003FF & a_idx);
				if (result >= DefaultObject::kTotalSE) {
					return kInvalid;
				}
			}

			return result ? result : kInvalid;
		}
	}

	TESForm* BGSDefaultObjectManager::GetObject(DefaultObjectID a_object) noexcept
	{
		auto idx = MapIndex(stl::to_underlying(a_object));
		assert(idx != kInvalid);
		if (idx == kInvalid) {
			return nullptr;
		}
		if (!IsObjectInitialized(a_object))
		{
			return nullptr;
		}
		
		auto* objectsxx = &RelocateMember<TESForm*>(this, 0x20, 0x20);

		if (!objectsxx) {
			return nullptr;
		}

		return objectsxx[idx];
	}

	bool BGSDefaultObjectManager::IsObjectInitialized(DefaultObjectID a_object) const noexcept
	{
		auto idx = MapIndex(stl::to_underlying(a_object));
		assert(idx != kInvalid);
		if (idx == kInvalid) {
			return false;
		}
		
		const auto* objectInitxx = &RelocateMember<bool>(this, 0xB80, 0xB90, 0xBA8);
		return objectInitxx[idx];
	}

	bool BGSDefaultObjectManager::SupportsVR(DefaultObjectID a_object) noexcept
	{
		auto idx = stl::to_underlying(a_object);
		return idx <= stl::to_underlying(DefaultObjectID::kKeywordActivatorFurnitureNoPlayer) || idx & 0x3FF00000;
	}

	bool BGSDefaultObjectManager::SupportsSE(DefaultObjectID a_object) noexcept
	{
		return ((stl::to_underlying(a_object) & 0x000003FF) || a_object == DefaultObjectID::kWerewolfSpell);
	}

	bool BGSDefaultObjectManager::SupportsAE(DefaultObjectID a_object) noexcept
	{
		auto idx = stl::to_underlying(a_object);
		return (idx <= stl::to_underlying(DefaultObjectID::kKeywordWeaponMaterialStalhrim) || (idx & 0x000FFC00) || a_object == DefaultObjectID::kWerewolfSpell);
	}

	bool BGSDefaultObjectManager::SupportsCurrentRuntime(DefaultObjectID a_object) noexcept
	{
		return MapIndex(stl::to_underlying(a_object)) != kInvalid;
	}
}
