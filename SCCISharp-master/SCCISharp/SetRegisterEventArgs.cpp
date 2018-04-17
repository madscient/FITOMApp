#include "stdafx.h"
#include "SCCISharp.h"

#include "scci/scci.h"

namespace SCCISharp {
	using namespace System;

	SCCISharp::SetRegisterEventArgs::SetRegisterEventArgs(int address, int data)
	{
		this->address = address;
		this->data = data;
	}

	int SCCISharp::SetRegisterEventArgs::Address::get()
	{
		return this->address;
	}

	int SCCISharp::SetRegisterEventArgs::Data::get()
	{
		return this->data;
	}
}