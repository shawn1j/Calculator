#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QString>
#include <QStack>

QT_BEGIN_NAMESPACE
class QToolButton;
class QLineEdit;
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT
public:
    enum class Mode
    {
        Number = 1,         // 数字
        Addition = 2,       // 加
        Subtraction = 3,    // 减
        Multiplication = 4, // 乘
        Division = 5,       // 除
        ClearAll,           // 清除
        Equal,              // 等于
        Percent,            // 百分号
        Square,             // 平方
        SquareRoot,         // 平方根
        Symbol,             // 符号
        Decimal,            // 小数点
        LeftParentheses,    // 左括号
        RightParentheses,   // 右括号
        Reciprocal,         // 倒数
        Factorial,          // 阶乘
        Log10,              // 自然对数
        Log2,               // 以2为底的对数
        Pi                  // 圆周率

    };

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();
public slots:
    void onToolButtonClicked(QToolButton *button, Mode mode);

public:
    void clear();

private:
    void initUI();
    QToolButton *createToolButton(const QString &text, Mode mode);
    double calculate(double left, double right);
    double calculateResult();
    void calculateMedian();
    bool maybecalculate();

public:
    static constexpr double pi = 3.141592653;

private:
    QLineEdit *display;
    QStack<double> number_stack;
    QStack<Mode> operator_stack;
    QString expression;
    QString last_number;
    bool clear_flag;
};
#endif // CALCULATOR_H
