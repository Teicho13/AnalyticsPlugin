// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"

UDataAnalyticsBPLibrary::UDataAnalyticsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


/**
* iterate through all properties of a struct
* @param prop    The struct property reflection data
* @param structPtr The pointer to the struct
*/
void UDataAnalyticsBPLibrary::DataToStringImpl(FProperty* prop, void* structPtr)
{
	//Reset all data like headers and value strings
	ResetValues();
	
	FStructProperty* StructProperty = CastField<FStructProperty>(prop);
	//Check if empty / null
	if(StructProperty)
	{
		//Go trough Struct
		for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
		{
			//Record header name
			HeaderNames.Append(*PropertyIt->GetAuthoredName());
			HeaderNames.Append(TEXT(", "));
		}

		//Remove last header comma
		int cutSize = HeaderNames.Len() - 2;
		HeaderNames.RemoveAt(cutSize);
	}
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	HeaderNames.Empty();
}

FString UDataAnalyticsBPLibrary::GetHeaderNames()
{
	return HeaderNames;
}


