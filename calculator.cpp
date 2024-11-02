#include "calculator.h"
#include <QToolButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <qmath.h>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent),
      display(new QLineEdit(this)),
      clear_flag(false)

{
    this->setMinimumSize(400, 620);
    this->initUI();
}

Calculator::~Calculator() {}

void Calculator::onToolButtonClicked(QToolButton *button, Mode mode)
{
    if (this->clear_flag)
    {
        this->expression.clear(); // 清除上一次计算结果
        clear();
    }

    switch (mode)
    {
    case Mode::Number:
        // 开头禁止连续输入多个0
        if (button->text() == "0" && this->last_number == "0")
            return;

        this->last_number += button->text();
        break;

    case Mode::Decimal:
        this->last_number += button->text();
        break;

    case Mode::Addition:
    case Mode::Subtraction:
    case Mode::Multiplication:
    case Mode::Division:
        // 操作数入栈
        if (!this->last_number.isEmpty())
        {
            this->number_stack.push(this->last_number.toDouble());
            // 更新显示的表达式
            this->expression = this->expression + this->last_number + button->text();

            this->last_number.clear();
        }
        // 运算符入栈
        this->operator_stack.push(mode);

        try
        {
            calculateMedian();
        }
        catch (QString e)
        {
            this->expression = e;
            this->clear_flag = true;
            goto UPDATE_DISPLAY;
        }

        break;
    case Mode::LeftParentheses:
        this->expression += button->text();
        this->operator_stack.push(mode);
        break;

    case Mode::RightParentheses:
        // 操作数入栈
        if (!this->last_number.isEmpty())
            this->number_stack.push(this->last_number.toDouble());

        // 运算符入栈
        this->operator_stack.push(mode);
        try
        {
            calculateMedian(); // 计算括号内的表达式
        }
        catch (QString e)
        {
            this->expression = e;
            this->clear_flag = true;
            goto UPDATE_DISPLAY;
        }
        this->expression = this->expression + this->last_number + button->text();
        this->last_number.clear();
        break;

    case Mode::Factorial:
        // 计算阶乘结果
        if (this->last_number.isEmpty())
            goto UPDATE_DISPLAY;

        this->last_number = QString::number(tgamma(this->last_number.toDouble() + 1));
        break;
    case Mode::Reciprocal:
        if (this->last_number.isEmpty())
            goto UPDATE_DISPLAY;

        this->last_number = QString::number(1 / this->last_number.toDouble());

        goto UPDATE_DISPLAY;

    case Mode::Log10:
        if (this->last_number.isEmpty())
            break;

        this->last_number = QString::number(log10(this->last_number.toDouble()));

        break;
    case Mode::Log2:
        if (this->last_number.isEmpty())
            break;

        this->last_number = QString::number(log2(this->last_number.toDouble()));

        break;

    case Mode::Square:
        if (this->last_number.isEmpty())
            goto UPDATE_DISPLAY;

        this->last_number = QString::number(qPow(this->last_number.toDouble(), 2));
        break;

    case Mode::SquareRoot:
        if (this->last_number.isEmpty())
            goto UPDATE_DISPLAY;
        this->last_number = QString::number(sqrt(this->last_number.toDouble()));

        break;

    case Mode::Pi:
        if (!this->last_number.isEmpty())
            this->last_number.clear();

        this->last_number = QString::number(Calculator::pi);
        goto UPDATE_DISPLAY;
    case Mode::Symbol:
        // 数字0不需要改变符号
        if (this->last_number == "0")
            return;

        // 正数变成负数
        if (this->last_number[0] != "-")
            this->last_number = "-" + this->last_number;
        // 负数变成正数
        else
            this->last_number = this->last_number.removeFirst();

        goto UPDATE_DISPLAY;

    case Mode::Percent:
        if (this->last_number == "0")
            return;

        this->last_number = QString::number(this->last_number.toDouble() / 100);

        goto UPDATE_DISPLAY;

    case Mode::Equal:
    {
        if (this->number_stack.isEmpty())
            goto UPDATE_DISPLAY;

        if (!this->last_number.isEmpty())
        {
            // 操作数入栈
            this->number_stack.push(this->last_number.toDouble());
        }
        // 计算结果
        double result;
        try
        {
            result = calculateResult();
        }
        catch (QString e)
        {
            this->expression = e;
            this->clear_flag = true;
            goto UPDATE_DISPLAY;
        }

        expression = expression + this->last_number + "=" + QString::number(result);
        this->last_number.clear();

        this->clear_flag = true;
        goto UPDATE_DISPLAY;
    }
    case Mode::ClearAll:
        this->expression.clear();
        this->last_number.clear();
        this->clear_flag = true;
        goto UPDATE_DISPLAY;
    }

UPDATE_DISPLAY:
    this->display->setText(expression + this->last_number);
}

void Calculator::clear()
{
    this->number_stack.clear();
    this->operator_stack.clear();
    this->expression.clear();
    this->last_number.clear();
    this->clear_flag = false;
}

void Calculator::initUI()
{
    auto *btnLayout = new QGridLayout;

    btnLayout->addWidget(createToolButton("Π", Mode::Pi), 0, 0);
    btnLayout->addWidget(createToolButton("n!", Mode::Factorial), 0, 1);
    btnLayout->addWidget(createToolButton("Log₂(n)", Mode::Log2), 0, 2);
    btnLayout->addWidget(createToolButton("Clear", Mode::ClearAll), 0, 3);

    btnLayout->addWidget(createToolButton("(", Mode::LeftParentheses), 1, 0);
    btnLayout->addWidget(createToolButton(")", Mode::RightParentheses), 1, 1);
    btnLayout->addWidget(createToolButton("Log₁₀(n)", Mode::Log10), 1, 2);
    btnLayout->addWidget(createToolButton("%", Mode::Percent), 1, 3);

    btnLayout->addWidget(createToolButton("⅟x", Mode::Reciprocal), 2, 0);
    btnLayout->addWidget(createToolButton("x²", Mode::Square), 2, 1);
    btnLayout->addWidget(createToolButton("²√x", Mode::SquareRoot), 2, 2);
    btnLayout->addWidget(createToolButton("÷", Mode::Division), 2, 3);

    btnLayout->addWidget(createToolButton("7", Mode::Number), 3, 0);
    btnLayout->addWidget(createToolButton("8", Mode::Number), 3, 1);
    btnLayout->addWidget(createToolButton("9", Mode::Number), 3, 2);
    btnLayout->addWidget(createToolButton("x", Mode::Multiplication), 3, 3);

    btnLayout->addWidget(createToolButton("4", Mode::Number), 4, 0);
    btnLayout->addWidget(createToolButton("5", Mode::Number), 4, 1);
    btnLayout->addWidget(createToolButton("6", Mode::Number), 4, 2);
    btnLayout->addWidget(createToolButton("-", Mode::Subtraction), 4, 3);

    btnLayout->addWidget(createToolButton("1", Mode::Number), 5, 0);
    btnLayout->addWidget(createToolButton("2", Mode::Number), 5, 1);
    btnLayout->addWidget(createToolButton("3", Mode::Number), 5, 2);
    btnLayout->addWidget(createToolButton("+", Mode::Addition), 5, 3);

    btnLayout->addWidget(createToolButton("±", Mode::Symbol), 6, 0);
    btnLayout->addWidget(createToolButton("0", Mode::Number), 6, 1);
    btnLayout->addWidget(createToolButton(".", Mode::Decimal), 6, 2);
    btnLayout->addWidget(createToolButton("=", Mode::Equal), 6, 3);

    this->display->setMinimumSize(400, 100);
    this->display->setReadOnly(true);
    this->display->setAlignment(Qt::AlignRight);
    auto font = this->display->font();
    font.setPointSize(30);
    this->display->setFont(font);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(display);
    mainLayout->addLayout(btnLayout);
}

QToolButton *Calculator::createToolButton(const QString &text, Mode mode)
{

    auto *button = new QToolButton(this);
    button->setText(text);
    button->setMinimumSize(100, 60);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);

    connect(button, &QToolButton::clicked, this, [=]()
            { onToolButtonClicked(button, mode); });
    return button;
}

double Calculator::calculate(double left, double right)
{
    double result;
    switch (this->operator_stack.top())
    {
    case Mode::Addition:
        qDebug() << QString::number(left) + "+" + QString::number(right);
        result = left + right;
        break;
    case Mode::Subtraction:
        qDebug() << QString::number(left) + "-" + QString::number(right);
        result = left - right;
        break;
    case Mode::Multiplication:
        qDebug() << QString::number(left) + "*" + QString::number(right);
        result = left * right;
        break;
    case Mode::Division:
        qDebug() << QString::number(left) + "/" + QString::number(right);
        result = left / right;
        break;
    default:
        throw QString("calculation error");
    }
    return result;
}

double Calculator::calculateResult()
{

    double rightOperand, leftOperand, result;
    while (!this->operator_stack.isEmpty())
    {
        if (this->operator_stack.size() >= 1 && this->number_stack.size() <= 1)
            throw QString("calculation error");

        rightOperand = this->number_stack.top();
        this->number_stack.pop();
        leftOperand = this->number_stack.top();
        this->number_stack.pop();

        result = calculate(leftOperand, rightOperand);

        this->operator_stack.pop();
        this->number_stack.push(result);
    }

    result = this->number_stack.top();

    return result;
}

void Calculator::calculateMedian()
{
    double rightOperand, leftOperand, result;
    while (!this->operator_stack.isEmpty() && maybecalculate())
    {

        if (this->operator_stack.size() >= 1 && this->number_stack.size() <= 1)
            throw QString("calculation error");

        rightOperand = this->number_stack.top();
        this->number_stack.pop();
        leftOperand = this->number_stack.top();
        this->number_stack.pop();

        Mode lastOperator = this->operator_stack.top();
        this->operator_stack.pop();

        result = calculate(leftOperand, rightOperand);

        this->operator_stack.pop();
        this->number_stack.push(result);
        this->operator_stack.push(lastOperator);
    }

    if (this->operator_stack.top() == Mode::RightParentheses)
    {
        qDebug() << "remove ) and (";
        this->operator_stack.pop();
        this->operator_stack.pop();
    }
}

void printmaybecalculate(Calculator::Mode lastOperator, Calculator::Mode calculateOperator)
{
    QString str = "lastOperator: ";
    switch (lastOperator)
    {
    case Calculator::Mode::Addition:
        str += "+    ";
        break;
    case Calculator::Mode::Subtraction:
        str += "-    ";
        break;
    case Calculator::Mode::Multiplication:
        str += "*    ";
        break;
    case Calculator::Mode::Division:
        str += "/    ";
        break;
    case Calculator::Mode::LeftParentheses:
        str += "(    ";
        break;
    case Calculator::Mode::RightParentheses:
        str += ")    ";
        break;
    }
    str += "calculateOperator: ";

    switch (calculateOperator)
    {
    case Calculator::Mode::Addition:
        str += "+    ";
        break;
    case Calculator::Mode::Subtraction:
        str += "-    ";
        break;
    case Calculator::Mode::Multiplication:
        str += "*    ";
        break;
    case Calculator::Mode::Division:
        str += "/    ";
        break;
    case Calculator::Mode::LeftParentheses:
        str += "(    ";
        break;
    case Calculator::Mode::RightParentheses:
        str += ")    ";
        break;
    }
    qDebug() << str;
}

bool Calculator::maybecalculate()
{
    if (this->operator_stack.size() < 2)
    {
        qDebug() << "maybecalculate: false";
        return false;
    }

    Mode lastOperator = this->operator_stack.top();
    this->operator_stack.pop();

    Mode calculateOperator = this->operator_stack.top();
    this->operator_stack.push(lastOperator);

    printmaybecalculate(lastOperator, calculateOperator);

    if ((lastOperator == Mode::Multiplication || lastOperator == Mode::Division) && (calculateOperator == Mode::Addition || calculateOperator == Mode::Subtraction))
    {
        qDebug() << "maybecalculate: false";
        return false;
    }
    else if (calculateOperator == Mode::LeftParentheses)
    {
        qDebug() << "maybecalculate: false ----- is ( operator";
        return false;
    }
    else if (lastOperator == Mode::RightParentheses)
    {
        qDebug() << "maybecalculate: true ----- is ) operator";
        return true;
    }

    qDebug() << "maybecalculate: true";
    return true;
}
