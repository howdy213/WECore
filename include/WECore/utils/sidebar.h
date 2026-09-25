/**
 * @file sidebar.h
 * @brief Vertical navigation bar that drives a QStackedWidget.
 *
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QButtonGroup>
#include <QGridLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QVector>
#include <QWidget>

/**
 * @brief A fixed-width strip of exclusive tool buttons, one per page, used to
 *        switch the current page of a connected QStackedWidget.
 *
 * Typical use: construct the bar, call setBtnCount() once, bind the stacked
 * widget with connectStack(), then label each button with setButtonContent().
 */
class SideBar : public QWidget {
    Q_OBJECT

public:
    explicit SideBar(QWidget *parent = nullptr);

    /// Rebuilds the bar with @p count exclusive buttons and checks button 0; meant to be called once.
    void setBtnCount(unsigned int count);

    /// Sets the label of button @p index.
    void setButtonContent(unsigned int index, QString content);

    /// Binds the stacked widget whose page is switched when a button is clicked.
    void connectStack(QStackedWidget *stackedWidget);

    ~SideBar();
private slots:
    void onButtonClicked();

private:
    QStackedWidget *stack = nullptr;
    QButtonGroup btnGroup;
};

#endif // SIDEBAR_H
