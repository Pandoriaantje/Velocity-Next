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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QPalette>
#include <QSettings>
#include <QApplication>
#include <QStyleHints>

/**
 * @brief Manages application theme and color scheme
 * 
 * Supports three theme modes:
 * - Auto: Follows system theme (Windows 10+, macOS 10.14+, modern Linux DEs)
 * - Dark: Forces dark mode
 * - Light: Forces light mode
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class ThemeMode {
        Auto,   ///< Follow system theme
        Dark,   ///< Force dark theme
        Light   ///< Force light theme
    };
    Q_ENUM(ThemeMode)

    /**
     * @brief Get the singleton instance
     */
    static ThemeManager& instance();

    /**
     * @brief Initialize theme system and apply saved theme
     */
    void initialize();

    /**
     * @brief Get current theme mode
     */
    ThemeMode themeMode() const { return m_themeMode; }

    /**
     * @brief Set theme mode and apply it
     */
    void setThemeMode(ThemeMode mode);

    /**
     * @brief Check if system theme detection is supported
     */
    bool systemThemeSupported() const;

    /**
     * @brief Get current effective theme (resolves Auto to actual Dark/Light)
     */
    bool isDarkTheme() const;

signals:
    /**
     * @brief Emitted when theme changes
     */
    void themeChanged(bool isDark);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager() = default;

    // Prevent copying
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    /**
     * @brief Apply dark theme palette
     */
    void applyDarkTheme();

    /**
     * @brief Apply light theme palette
     */
    void applyLightTheme();

    /**
     * @brief Apply theme based on current mode
     */
    void applyTheme();

    /**
     * @brief Save theme mode to settings
     */
    void saveThemeMode();

    /**
     * @brief Load theme mode from settings
     */
    void loadThemeMode();

private slots:
    /**
     * @brief Handle system theme changes (when mode is Auto)
     */
    void onSystemThemeChanged();

private:
    ThemeMode m_themeMode;
    QSettings* m_settings;
    bool m_systemThemeSupported;
};

#endif // THEMEMANAGER_H
