// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataAnalyticsBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UDataAnalyticsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, CustomThunk , meta = (CustomStructureParam = "InStruct"))
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

		DataToStringImpl(StructProperty,StructPtr);
	}

	static void DataToStringImpl(FProperty* prop, void* structPtr);

	static void ParseStructData(FProperty* prop, void* valuePtr);

	static void ResetValues();

	UFUNCTION(BlueprintCallable)
	static FString GetHeaderNames();

	UFUNCTION(BlueprintCallable)
	static FString GetValues();
	
	static FString HeaderNames;
	static FString Values;
};

	FString UDataAnalyticsBPLibrary::HeaderNames = "";
	FString UDataAnalyticsBPLibrary::Values = "";
