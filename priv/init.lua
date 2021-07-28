celerygame.app = "whatever"
celerygame.vsn = 8192

function celerygame.init_callback()
    celerygame:init_vulkan()
end

function celerygame.runloop_callback()
    while true do
        local event = celerygame:poll_event()
        if event then
            if event.type == "quit" then
                return true
            end
        else
            break
        end
    end
    return false
end

function celerygame.deinit_callback()
    celerygame:deinit_vulkan()
end

print("ok")