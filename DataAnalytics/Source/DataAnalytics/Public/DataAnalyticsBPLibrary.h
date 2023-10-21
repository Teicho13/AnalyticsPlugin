// Copyright Epic Games, Inc. All Rights Reserved.
//Implementation wilcard struct: https://forums.unrealengine.com/t/tutorial-how-to-accept-wildcard-structs-in-your-ufunctions/18968/5

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataAnalyticsBPLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AnalyticsLog, Verbose, All);

UCLASS()
class UDataAnalyticsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Takes struct and returns the data into a string
	* @param InStruct  Any struct to convert
	*/
	UFUNCTION(BlueprintCallable, CustomThunk , meta = (CustomStructureParam = "InStruct"), Category="DataAnalytics | Data")
	static void DataToString( UStruct* InStruct);
	
	DECLARE_FUNCTION(execDataToString)
	{
		// Steps into the stack, walking to the next property in it
		Stack.Step(Stack.Object, NULL);

		// Grab the last property found when we walked the stack
		// This does not contains the property value, only its type information
		FProperty* StructProperty = CastField<FProperty>(Stack.MostRecentProperty);

		// Grab the base address where the struct actually stores its data
		// This is where the property value is truly stored
		void* StructPtr = Stack.MostRecentPropertyAddress;

		// We need this to wrap up the stack
		P_FINISH;

		//Reset all data like headers and value strings
		ResetValues();
		
		DataToStringImpl(StructProperty,StructPtr, 0);
	}

	//Return output string with all headers and values
	UFUNCTION(BlueprintCallable)
	static FString GetAnalyticsOutput();

	/**
	* Writes data string into a csv file
	* @param FilePath Location file will be generated
	* @param InString Input values
	*/
	UFUNCTION(BlueprintCallable, Category="DataAnalytics | FileIO")
	static void WriteToCSV(FString FilePath, FString InString, bool& outSuccess);

private:
	
	/**
	* iterate through all properties of a struct
	* @param prop    The struct property reflection data
	* @param structPtr The pointer to the struct
	*/
	static void DataToStringImpl(FProperty* prop, void* structPtr, int layer);
	
	/**
	* Parse given property data 
	* @param prop The property reflection data
	* @param valuePtr The pointer to the struct value
	*/
	static FString ParseStructData(FProperty* prop, void* valuePtr, int layer);

	//Reset all values (strings)
	static void ResetValues();
	
	static FString FinalString;
	static FString HeaderString;
	static FString ValueString;

};
	FString UDataAnalyticsBPLibrary::FinalString = "";
	FString UDataAnalyticsBPLibrary::HeaderString = "";
	FString UDataAnalyticsBPLibrary::ValueString = "";

