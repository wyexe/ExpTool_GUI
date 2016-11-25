#include "stdafx.h"
#include "ExpTool.h"

BOOL CExpTool::GetResult(_In_ cwstring& wsExpressionText) throw()
{
	Initialize();
	if (!SetExpression(wsExpressionText))
		return FALSE;

	SetRpn();

	return SetResult();
}

BOOL CExpTool::SetExpression(_In_ cwstring& wsText) throw()
{
	std::wstring wsNumber;
	em_Text_Type emLastTextType = em_Text_Type::em_Text_Type_NULL;
	BOOL bNegative = FALSE;
	for (CONST auto& wchText : wsText)
	{
		auto emTextType = GetTextType(wchText);
		switch (emTextType)
		{
		case em_Text_Type_Invalid:
			wsErrText = L"无效的字符'";
			wsErrText.push_back(wchText);
			wsErrText += L"'";
			return FALSE;
		case em_Text_Type_Number:
			wsNumber.push_back(wchText);
			break;
		case em_Text_Type_Symbol: case em_Text_Type_Bracket:
		{
			if (!wsNumber.empty())
			{
				if (bNegative)
					wsNumber = L"-" + wsNumber;

				Exp_.push_back(ExpressionContent{ em_Content_Type_Number_Float, wsNumber });
				wsNumber.clear();
				bNegative = FALSE;
			}

			// 2次练习出现符号, 这次的符号必须是-的才行
			if (emTextType != em_Text_Type_Bracket && emLastTextType == em_Text_Type_Symbol)
			{
				if (wchText != L'-')
				{
					wsErrText = L"连续出现操作符, 这是不合法的! '";
					wsErrText.push_back(wchText);
					wsErrText += L"'";
					return FALSE;
				}
				bNegative = TRUE;
				break;
			}


			std::wstring wsContentText;
			wsContentText.push_back(wchText);
			auto emContentType = GetContentType(wsContentText, emTextType);
			if (emContentType == em_Content_Type::em_Content_Type_None)
			{
				wsErrText = L"无效的字符'";
				wsErrText.push_back(wchText);
				wsErrText += L"'";
				return FALSE;
			}

			Exp_.push_back(ExpressionContent{ emContentType, wsContentText });
		}
		break;
		case em_Text_Type_NULL:
			break;
		default:
			break;
		}
		emLastTextType = emTextType;
	}

	if (!wsNumber.empty())
	{
		if (bNegative)
			wsNumber = L"-" + wsNumber;

		Exp_.push_back(ExpressionContent{ em_Content_Type_Number_Float, wsNumber });
		wsNumber.clear();
	}

	return TRUE;
}

CExpTool::em_Text_Type CExpTool::GetTextType(_In_ WCHAR wchText) throw()
{
	static CONST WCHAR wszNumberText[] = { L"0123456789." };
	static CONST WCHAR wszSymbolText[] = { L"+-*/" };

	if (std::find_if(std::begin(wszNumberText), std::end(wszNumberText), [wchText](WCHAR wch){ return wch == wchText; }) != std::end(wszNumberText))
		return em_Text_Type::em_Text_Type_Number;

	if (std::find_if(std::begin(wszSymbolText), std::end(wszSymbolText), [wchText](WCHAR wch){ return wch == wchText; }) != std::end(wszSymbolText))
		return em_Text_Type::em_Text_Type_Symbol;

	if (wchText == L'(' || wchText == L')')
		return em_Text_Type_Bracket;

	if (wchText == L' ' || wchText == L'\0' || wchText == L'\r' || wchText == L'\n')
		return em_Text_Type::em_Text_Type_NULL;

	return em_Text_Type::em_Text_Type_Invalid;
}

CExpTool::em_Content_Type CExpTool::GetContentType(_In_ cwstring& wsText, _In_ em_Text_Type emTextType) throw()
{
	struct TextContent
	{
		em_Content_Type emContentType;
		std::wstring    wsText;
	};

	if (emTextType == em_Text_Type::em_Text_Type_Symbol)
	{
		CONST static std::vector<TextContent> Vec =
		{
			{ em_Content_Type::em_Content_Type_Symbol_Add, L"+" },
			{ em_Content_Type::em_Content_Type_Symbol_Sub, L"-" },
			{ em_Content_Type::em_Content_Type_Symbol_Mul, L"*" },
			{ em_Content_Type::em_Content_Type_Symbol_Div, L"/" },
		};

		auto p = CLPublic::Vec_find_if_Const(Vec, [wsText](CONST TextContent& TC){ return TC.wsText == wsText; });
		return p != nullptr ? p->emContentType : em_Content_Type::em_Content_Type_None;
	}
	else if (emTextType == em_Text_Type::em_Text_Type_Bracket)
	{
		CONST static std::vector<TextContent> Vec =
		{
			{ em_Content_Type::em_Content_Type_LeftBracket, L"(" },
			{ em_Content_Type::em_Content_Type_RightBracket, L")" },
		};

		auto p = CLPublic::Vec_find_if_Const(Vec, [wsText](CONST TextContent& TC){ return TC.wsText == wsText; });
		return p != nullptr ? p->emContentType : em_Content_Type::em_Content_Type_None;
	}
	return em_Content_Type::em_Content_Type_None;
}

VOID CExpTool::Initialize() throw()
{
	VecCalcProcess.clear();

	Exp_.clear();

	wsErrText.clear();

	while (!Rpn.empty())
		Rpn.pop();

}

VOID CExpTool::SetRpn() throw()
{
	std::stack<ExpressionContent> StackSymbol;
	std::stack<ExpressionContent> tmpRpn;
	for (CONST auto& itm : Exp_)
	{
		if (itm.emContentType == em_Content_Type::em_Content_Type_Number_Float)
		{
			tmpRpn.push(itm);
			continue;
		}

		if (itm.emContentType == em_Content_Type::em_Content_Type_LeftBracket)
		{
			StackSymbol.push(itm);
			continue;
		}

		if (itm.emContentType == em_Content_Type::em_Content_Type_RightBracket)
		{
			while (StackSymbol.top().emContentType != em_Content_Type::em_Content_Type_LeftBracket)
			{
				tmpRpn.push(std::move(StackSymbol.top()));
				StackSymbol.pop();
			}
			StackSymbol.pop();
			continue;
		}

		if (StackSymbol.empty() || StackSymbol.top().emContentType == em_Content_Type::em_Content_Type_LeftBracket)
		{
			StackSymbol.push(itm);
			continue;
		}

		if (GetPriority(itm.emContentType) > GetPriority(StackSymbol.top().emContentType))
		{
			StackSymbol.push(itm);
			continue;
		}

		tmpRpn.push(std::move(StackSymbol.top()));
		StackSymbol.pop();
		StackSymbol.push(itm);
	}

	while (!StackSymbol.empty())
	{
		tmpRpn.push(std::move(StackSymbol.top()));
		StackSymbol.pop();
	}

	while (!tmpRpn.empty())
	{
		Rpn.push(std::move(tmpRpn.top()));
		tmpRpn.pop();
	}
}

int CExpTool::GetPriority(_In_ em_Content_Type emContentType) CONST throw()
{
	switch (emContentType)
	{
	case em_Content_Type_Symbol_Add: case em_Content_Type_Symbol_Sub:
		return 1;
	case em_Content_Type_Symbol_Mul: case em_Content_Type_Symbol_Div:
		return 2;
	default:
		break;
	}
	return 0;
}

BOOL CExpTool::SetResult() throw()
{
	std::stack<double> StackResult;
	while (!Rpn.empty())
	{
		auto& itm = Rpn.top();
		if (itm.emContentType == em_Content_Type::em_Content_Type_Number_Float)
		{
			StackResult.push(itm.GetNumber());
			Rpn.pop();
			continue;
		}
		if (StackResult.size() <= 1)
		{
			wsErrText = L"UnExist Number calc!";
			return FALSE;
		}

		double NumberRight = StackResult.top();
		StackResult.pop();
		if (StackResult.size() == 0)
		{
			wsErrText = L"calc Number have to two Number at least! ";
			return FALSE;
		}

		double NumberLeft = StackResult.top();
		StackResult.pop();

		if (itm.emContentType == em_Content_Type::em_Content_Type_Symbol_Div && NumberRight == 0)
		{
			wsErrText = L"are u kidding me? did u want to div zero?";
			return FALSE;
		}

		double nResult = CalcResult(NumberLeft, NumberRight, itm.emContentType);
		
		wstream.clear();
		wstream.str(L"");
		wstream << NumberLeft << L" " << itm.wsText.c_str() << L" " << NumberRight << L" = " << nResult << std::endl;
		VecCalcProcess.push_back(wstream.str());

		StackResult.push(nResult);
		Rpn.pop();
	}

	if (StackResult.size() == 0)
	{
		wsErrText = L"UnExist Result!";
		return FALSE;
	}

	wstream.clear();
	wstream.str(L"");
	wstream << L"Result = " << StackResult.top();
	VecCalcProcess.push_back(wstream.str());
	return TRUE;
}

double CExpTool::CalcResult(_In_ double NumberLeft, _In_ double NumberRight, _In_ em_Content_Type emSymbol)
{
	switch (emSymbol)
	{
	case em_Content_Type_Symbol_Add:
		return NumberLeft + NumberRight;
	case em_Content_Type_Symbol_Sub:
		return NumberLeft - NumberRight;
	case em_Content_Type_Symbol_Mul:
		return NumberLeft * NumberRight;
	case em_Content_Type_Symbol_Div:
		return NumberLeft / NumberRight;
	default:
		break;
	}
	return 0;
}

cwstring& CExpTool::GetErrText() CONST throw()
{
	return wsErrText;
}

CONST std::vector<std::wstring> CExpTool::GetCalcPrcoess() CONST throw()
{
	return VecCalcProcess;
}

