#include "RE/S/SkyrimVM.h"
#include "RE/V/VirtualMachine.h"

namespace RE
{
	SkyrimVM* SkyrimVM::GetSingleton()
	{
		static REL::Relocation<SkyrimVM**> singleton{ RELOCATION_ID(514315, 400475) };
		return *singleton;
	}

	BSTSmartPointer<BSScript::IVirtualMachine>& SkyrimVM::GetImpl() noexcept
	{
		const auto offset = REL::Module::IsAE() && REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_7_99 ? 0x210 : 0x200;
		return REL::RelocateMember<BSTSmartPointer<BSScript::IVirtualMachine>>(this, offset);
	}

	const BSTSmartPointer<BSScript::IVirtualMachine>& SkyrimVM::GetImpl() const noexcept
	{
		const auto offset = REL::Module::IsAE() && REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_7_99 ? 0x210 : 0x200;
		return REL::RelocateMember<const BSTSmartPointer<BSScript::IVirtualMachine>>(this, offset);
	}

	bool SkyrimVM::QueuePostRenderCall(const BSTSmartPointer<SkyrimScript::DelayFunctor>& a_functor)
	{
		using func_t = decltype(&SkyrimVM::QueuePostRenderCall);
		static REL::Relocation<func_t> func{ RELOCATION_ID(53144, 53955) };
		return func(this, a_functor);
	}

	void SkyrimVM::RelayEvent(VMHandle a_handle, BSFixedString* a_event, BSScript::IFunctionArguments* a_args, SkyrimVM::ISendEventFilter* a_optionalFilter)
	{
		using func_t = decltype(&SkyrimVM::RelayEvent);
		static REL::Relocation<func_t> func{ RELOCATION_ID(53221, 54033) };
		return func(this, a_handle, a_event, a_args, a_optionalFilter);
	}

	// a_handle must be a handle for TESObjectREFR
	// Sends event to handle directly, then relays event to all reference aliases and magic effects attached to reference
	void SkyrimVM::SendAndRelayEvent(VMHandle a_handle, BSFixedString* a_event, BSScript::IFunctionArguments* a_args, SkyrimVM::ISendEventFilter* a_optionalFilter)
	{
		GetImpl().get()->SendEvent(a_handle, *a_event, a_args);
		RelayEvent(a_handle, a_event, a_args, a_optionalFilter);
	}

	void SkyrimVM::Freeze()
	{
		using func_t = decltype(&SkyrimVM::Freeze);
		static REL::Relocation<func_t> func{ RELOCATION_ID(53204, 54015) };
		return func(this);
	}
}
