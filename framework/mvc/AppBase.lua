
local AppBase = class("AppBase")

AppBase.APP_ENTER_BACKGROUND_EVENT = "APP_ENTER_BACKGROUND_EVENT"
AppBase.APP_ENTER_FOREGROUND_EVENT = "APP_ENTER_FOREGROUND_EVENT"

function AppBase:ctor(appName, packageRoot)
    cc.GameObject.extend(self)
    self:addComponent("components.behavior.EventProtocol"):exportMethods()

    self.name = appName
    self.packageRoot = packageRoot or "app"

    local notificationCenter = CCNotificationCenter:sharedNotificationCenter()
    notificationCenter:registerScriptObserver(nil, handler(self, self.onEnterBackground_), "APP_ENTER_BACKGROUND_EVENT")
    notificationCenter:registerScriptObserver(nil, handler(self, self.onEnterForeground_), "APP_ENTER_FOREGROUND_EVENT")

    self.snapshots_ = {}

    -- set global app
    app = self
end

function AppBase:run()
end

function AppBase:exit()
    os.exit()
end

function AppBase:enterScene(sceneName, ...)
    local scenePackageName = self. packageRoot .. ".scenes." .. sceneName
    local sceneClass = require(scenePackageName)
    local scene = sceneClass.new(...)
    display.replaceScene(scene)
    collectgarbage("collect")
    collectgarbage("collect")
end

function AppBase:createView(viewName, ...)
    local viewPackageName = self. packageRoot .. ".views." .. viewName
    local viewClass = require(viewPackageName)
    return viewClass.new(...)
end

function AppBase:makeLuaVMSnapshot()
    self.snapshots_[#self.snapshots_ + 1] = CCLuaStackSnapshot()
    while #self.snapshots_ > 2 do
        table.remove(self.snapshots_, 1)
    end
end

function AppBase:checkLuaVMLeaks()
    assert(#self.snapshots_ >= 2, "AppBase:checkLuaVMLeaks() - need least 2 snapshots")
    local s1 = self.snapshots_[1]
    local s2 = self.snapshots_[2]
    for k, v in pairs(s2) do
        if s1[k] == nil then
            print(k, v)
        end
    end
end

function AppBase:onEnterBackground_()
    self:dispatchEvent({name = AppBase.APP_ENTER_BACKGROUND_EVENT})
end

function AppBase:onEnterForeground_()
    self:dispatchEvent({name = AppBase.APP_ENTER_FOREGROUND_EVENT})
end

return AppBase
