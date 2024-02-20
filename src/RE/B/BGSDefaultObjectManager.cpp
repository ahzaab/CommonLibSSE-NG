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
			// VR-AE-SE each place occupies 10 bits
			// The MSB indicates that the data is exclusively
			// AE data, otherwise tbe values are the same as SE

			std::size_t result;
			if SKYRIM_REL_CONSTEXPR (Module::IsVR()) {
				result = (0x3FF00000 & a_idx) >> 20;
			} else if SKYRIM_REL_CONSTEXPR (Module::IsAE()) {
				if (a_idx & 0x80000000) {
					result = (0x000FFC00 & a_idx) >> 10;
				} else {
					result = (0x00003FF & a_idx);
				}
			} else {
				result = (0x00003FF & a_idx);
			}
			return result ? result : kInvalid;
		}
	}

	TESForm* BGSDefaultObjectManager::GetObject(DefaultObjectID a_object) noexcept
	{
		assert(stl::to_underlying(a_object) < Relocate(364u, 364u, 369u));
		auto idx = MapIndex(stl::to_underlying(a_object));
		if (idx == kInvalid) {
			return nullptr;
		}
		return RelocateMember<bool*>(this, 0xB80u, 0xB98u, 0xBA8u)[idx] ?
                   RelocateMember<TESForm**>(this, 0x20, 0x20)[idx] :
                   nullptr;
	}

	bool BGSDefaultObjectManager::IsObjectInitialized(DefaultObjectID a_object) const noexcept
	{
		return IsObjectInitialized(MapIndex(stl::to_underlying(a_object)));
	}

	bool BGSDefaultObjectManager::SupportsVR(DefaultObjectID a_object) noexcept
	{
		auto idx = stl::to_underlying(a_object);
		return idx <= stl::to_underlying(DefaultObjectID::kKeywordActivatorFurnitureNoPlayer) || idx & 0x3FF00000;
	}

	bool BGSDefaultObjectManager::SupportsSE(DefaultObjectID a_object) noexcept
	{
		return (stl::to_underlying(a_object) & 0x000003FF) || a_object != DefaultObjectID::kWerewolfSpell;
	}

	bool BGSDefaultObjectManager::SupportsCurrentRuntime(DefaultObjectID a_object) noexcept
	{
		return MapIndex(stl::to_underlying(a_object)) != kInvalid;
	}
}
