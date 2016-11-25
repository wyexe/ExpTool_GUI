#ifndef __EXPTOOL_GUI_EXPRESSION_EXPTOOL_H__
#define __EXPTOOL_GUI_EXPRESSION_EXPTOOL_H__

#include <stack>
#include <string>
#include <sstream>
#include <MyTools/Character.h>
#include <MyTools/CLPublic.h>
#include <MyTools/ClassInstance.h>


class CExpTool : public CClassInstance<CExpTool>
{
public:
	CExpTool() = default;
	~CExpTool() = default;
private:
	enum em_Content_Type
	{
		em_Content_Type_None,
		// 1.0
		em_Content_Type_Number_Float,
		// (
		em_Content_Type_LeftBracket,
		// )
		em_Content_Type_RightBracket,
		// +
		em_Content_Type_Symbol_Add,
		// -
		em_Content_Type_Symbol_Sub,
		// *
		em_Content_Type_Symbol_Mul,
		// /
		em_Content_Type_Symbol_Div,
	};

	enum em_Text_Type
	{
		em_Text_Type_Invalid,
		em_Text_Type_Number,
		em_Text_Type_Symbol,
		em_Text_Type_Bracket,
		em_Text_Type_NULL
	};

	struct ExpressionContent
	{
		em_Content_Type emContentType;
		std::wstring wsText;
		double GetNumber()
		{
			return _wtof(wsText.c_str());
		}
	};

	typedef std::vector<ExpressionContent> Expression;

public:
	BOOL GetResult(_In_ cwstring& wsExpressionText) throw();

	cwstring& GetErrText() CONST throw();

	CONST std::vector<std::wstring> GetCalcPrcoess() CONST throw();
private:
	// decompose Expression
	BOOL SetExpression(_In_ cwstring& wsText) throw();

	// ')' => em_Text_Type_Bracket
	em_Text_Type GetTextType(_In_ WCHAR wchText) throw();

	// '+' => em_Content_Type_Symbol_Add
	em_Content_Type GetContentType(_In_ cwstring& wsText, _In_ em_Text_Type emTextType) throw();

	// Clear old mem
	VOID Initialize() throw();

	// Set Rpn by Exp_
	VOID SetRpn() throw();

	// Get Symbol Priority
	int GetPriority(_In_ em_Content_Type emContentType) CONST throw();

	// Set Result by Rpn
	BOOL SetResult() throw();

	double CalcResult(_In_ double NumberLeft, _In_ double NumberRight, _In_ em_Content_Type emSymbol);

private:
	std::wstring wsErrText;
	Expression Exp_;
	std::stack<ExpressionContent> Rpn;
	std::vector<std::wstring> VecCalcProcess;
	std::wostringstream wstream;
};



#endif