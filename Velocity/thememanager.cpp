/*
 * Velocity-Next - Xbox 360 File Manager
 * 
 * Copyright (C) 2025-present Velocity-Next Contributors
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "thememanager.h"
#include <QDebug>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_themeMode(ThemeMode::Light)
    , m_settings(new QSettings("Exetelek", "Velocity", this))
    , m_systemThemeSupported(false)
{
    // Check if system theme detection is available
    m_systemThemeSupported = (QApplication::styleHints()->colorScheme() != Qt::ColorScheme::Unknown);
    
    if (!m_systemThemeSupported) {
        qDebug() << "System theme detection not available on this platform";
    }
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

void ThemeManager::initialize()
{
    // Load saved theme mode
    loadThemeMode();
    
    // Apply the theme
    applyTheme();
    
    // If in Auto mode and system theme is supported, listen for changes
    if (m_themeMode == ThemeMode::Auto && m_systemThemeSupported) {
        connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                this, &ThemeManager::onSystemThemeChanged);
    }
}

bool ThemeManager::systemThemeSupported() const
{
    return m_systemThemeSupported;
}

bool ThemeManager::isDarkTheme() const
{
    switch (m_themeMode) {
        case ThemeMode::Dark:
            return true;
        case ThemeMode::Light:
            return false;
        case ThemeMode::Auto:
            if (m_systemThemeSupported) {
                return QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
            }
            return false; // Default to light if system theme not supported
    }
    return false;
}

void ThemeManager::setThemeMode(ThemeMode mode)
{
    if (m_themeMode == mode) {
        return;
    }
    
    // If switching from Auto mode, disconnect system theme listener
    if (m_themeMode == ThemeMode::Auto && m_systemThemeSupported) {
        disconnect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                   this, &ThemeManager::onSystemThemeChanged);
    }
    
    m_themeMode = mode;
    
    // If switching to Auto mode, connect system theme listener
    if (m_themeMode == ThemeMode::Auto && m_systemThemeSupported) {
        connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                this, &ThemeManager::onSystemThemeChanged);
    }
    
    // Apply the new theme
    applyTheme();
    
    // Save the preference
    saveThemeMode();
}

void ThemeManager::applyDarkTheme()
{
    qDebug() << "Applying dark theme";
    
    // Use Fusion style for consistent look
    QApplication::setStyle("Fusion");
    
    // Create dark palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Text, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
    
    // Disabled colors
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    
    QApplication::setPalette(darkPalette);
    
    emit themeChanged(true);
}

void ThemeManager::applyLightTheme()
{
    qDebug() << "Applying light theme";
    
    // Use Fusion style for consistent look
    QApplication::setStyle("Fusion");
    
    // Create light palette
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Base, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::AlternateBase, QColor(230, 230, 230));
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Text, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::Button, QColor(200, 200, 200));
    lightPalette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    lightPalette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
    lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    lightPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    
    // Disabled colors
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    lightPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(200, 200, 200));
    lightPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    
    QApplication::setPalette(lightPalette);
    
    emit themeChanged(false);
}

void ThemeManager::applyTheme()
{
    if (isDarkTheme()) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
}

void ThemeManager::saveThemeMode()
{
    QString modeStr;
    switch (m_themeMode) {
        case ThemeMode::Auto:
            modeStr = "auto";
            break;
        case ThemeMode::Dark:
            modeStr = "dark";
            break;
        case ThemeMode::Light:
            modeStr = "light";
            break;
    }
    m_settings->setValue("theme/mode", modeStr);
}

void ThemeManager::loadThemeMode()
{
    QString modeStr = m_settings->value("theme/mode", "light").toString();
    
    if (modeStr == "auto") {
        m_themeMode = ThemeMode::Auto;
    } else if (modeStr == "dark") {
        m_themeMode = ThemeMode::Dark;
    } else {
        m_themeMode = ThemeMode::Light;
    }
}

void ThemeManager::onSystemThemeChanged()
{
    if (m_themeMode == ThemeMode::Auto) {
        qDebug() << "System theme changed, reapplying theme";
        applyTheme();
    }
}
