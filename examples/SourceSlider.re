open Revery.Core;
open Revery.Core.Window;
open Revery.Core.Events;
open Revery.UI;
/* open Revery.UI.Components; */

type state = {
  sources: list(string),
  current: int,
  animation: Animated.animationValue,
};

type action =
  | Left
  | Right;

let offset = (1920 - 350) / 2;

let reducer = (action, state) =>
  switch (action) {
  | Left =>
    let newCurrent =
      state.current == 0 ? List.length(state.sources) - 1 : state.current - 1;
    let _ =
      Animated.start(
        state.animation,
        {
          toValue: float(newCurrent) *. (-350.) +. float(offset),
          duration: Seconds(0.3),
          delay: Seconds(0.),
          repeat: false,
          easing: Animated.easeInOut,
        },
      );
    {...state, current: newCurrent};
  | Right =>
    let newCurrent =
      state.current == List.length(state.sources) - 1 ? 0 : state.current + 1;
    let _ =
      Animated.start(
        state.animation,
        {
          toValue: float(newCurrent) *. (-350.) +. float(offset),
          duration: Seconds(0.3),
          delay: Seconds(0.),
          repeat: false,
          easing: Animated.easeInOut,
        },
      );
    {...state, current: newCurrent};
  };

let component = React.component("SourceSlider");

let wrapperStyle = Style.[flexDirection(`Row), alignItems(`Center)];

let make = window =>
  component(slots => {
    let ({sources, animation, _}, dispatch, slots) =
      React.Hooks.reducer(
        ~initialState={
          sources: [
            "Live TV",
            "HDMI 1",
            "Chromecast",
            "HDMI 3",
            "Recordings",
          ],
          current: 0,
          animation: Animated.floatValue(float(offset)),
        },
        reducer,
        slots,
      );

    let keyHandler = e =>
      switch (e.key) {
      | Key.KEY_LEFT => dispatch(Left)
      | Key.KEY_RIGHT => dispatch(Right)
      | _ => ()
      };

    let _slots: React.Hooks.empty =
      React.Hooks.effect(
        OnMount,
        () => {
          let unsubscribe = Event.subscribe(window.onKeyDown, keyHandler);
          Some(unsubscribe);
        },
        slots,
      );

    <View style=wrapperStyle>
      ...{
           sources
           |> List.map(source =>
                <View
                  style=Style.[
                    width(350),
                    height(100),
                    justifyContent(`Center),
                    alignItems(`Center),
                    transform([Transform.TranslateX(animation.current)]),
                  ]>
                  <Text
                    style=Style.[
                      fontSize(60),
                      fontFamily("Roboto-Regular.ttf"),
                      color(Colors.white),
                    ]
                    text=source
                  />
                </View>
              )
         }
    </View>;
  });

let createElement = (~window, ~children as _, ()) =>
  React.element(make(window));