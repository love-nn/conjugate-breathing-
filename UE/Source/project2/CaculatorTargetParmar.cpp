// Fill out your copyright notice in the Description page of Project Settings.


#include "CaculatorTargetParmar.h"

float UCaculatorTargetParmar::CalculateTargetParam(float CurBreathRate, float CurParam, float MinVal, float MaxVal) {
	float scope = MaxVal - MinVal;
	float dif = CurBreathRate - 0.1;
	return MaxVal - dif * scope;
}

float UCaculatorTargetParmar::CalculateNewParam(float CurParam, float TarParam, float inc) {
	float dif = (TarParam - CurParam);
	if (dif >= 0) {
		if (CurParam + inc < TarParam)
			return CurParam + inc;
		else
			return TarParam;
	}
	else {
		if (CurParam - inc > TarParam) {
			return TarParam;
		}
		else
			return CurParam - inc;
	}
}
bool UCaculatorTargetParmar::IsVaild(float CurParam, float Min, float Max) {
	if (CurParam >= Min && CurParam <= Max) {
		return true;
	}
	return false;
}