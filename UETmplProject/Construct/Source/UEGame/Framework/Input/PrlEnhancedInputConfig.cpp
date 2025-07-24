// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Input/PrlEnhancedInputConfig.h"

void UPrlEnhancedInputConfig::SaveConfigMappings()
{
	m_tarrActionMappings.Sort();
	m_tarrAxisMappings.Sort();
	SaveConfig();
}