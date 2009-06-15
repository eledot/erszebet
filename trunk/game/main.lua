#!/usr/bin/env lua-5.1

function g_shutdown()
end

print("initializing...")

jump = {}

function g_mouse_event(button, down, x, y)
   if button == 1 then
      if down > 0 then
         local ents = phys_point_query({x, y})
         if ents[1] then
            print("grabbed")
            jump = {ents[1], x, y}
         else
            jump = {}
         end
      elseif jump[1] then
         print("jumped")
         phys_apply_impulse(jump[1],
                            {jump[2] - jump[1].origin_x, jump[3] - jump[1].origin_y},
                            {3 * (x - jump[2]), 3 * (y - jump[3])})
      end
   elseif button == 3 then
      if down > 0 then
         local ents = phys_point_query({x, y})
         if ents[1] then
            print("boom!")
            remove(ents[1])
         end
      end
   elseif button == 2 then
      if down > 0 then
         print("spawn")
         spawn_T(x, y)
      end
   end
end

function spawn()
   local e = { }
   ent_spawn(e)
   return e
end

function remove(e)
   if e.remove then
      e.remove(e)
   end
   ent_remove(e)
end

local S = 10

function spawn_T(x, y)
   local ent1 = spawn()
   ent1.classname = "T"
   phys_set_body(ent1, BODY_POLYGON,
                 { -3*S, 3*S, 3*S, 3*S, 3*S, 1*S, -3*S, 1*S },
                 { -1*S, 1*S, 1*S, 1*S, 1*S, -5*S, -1*S, -5*S })
   ent1.origin = {x, y}
end

function spawn_Tb(x, y)
   local ent1 = spawn()
   ent1.classname = "T.1"
   phys_set_body(ent1, BODY_POLYGON_CENTERED,
                 { -3*S, 3*S, 3*S, 3*S, 3*S, 1*S, -3*S, 1*S })
   ent1.origin = {x, y + 40}

   local ent2 = spawn()
   ent2.classname = "T.2"
   phys_set_body(ent2, BODY_POLYGON_CENTERED,
                 { -1*S, 1*S, 1*S, 1*S, 1*S, -5*S, -1*S, -5*S })
   ent2.origin = {x, y}
   phys_attach_pin(ent1, ent2)
end

local ent3 = spawn()
phys_set_body(ent3, BODY_CIRCLE, 10)
ent3.classname = "shit"
ent3.origin = {550, 100}
ent3.velocity = { -450, 0 }
ent3.mass = 10
ent3.elasticity = 1
ent3.gravity = -300
ent3.touch = function (self, other, origin, normal)
                if other then
                   phys_detach(other)
                end
                return 1
             end

spawn_Tb(275, 190)
spawn_Tb(275, 90)
spawn_Tb(195, 190)
spawn_Tb(195, 90)
spawn_Tb(115, 190)
spawn_Tb(115, 90)

print("done")
