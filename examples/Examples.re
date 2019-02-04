open Revery;
open Revery.Core;
/* open Revery.Math; */

module SliderExample = Slider;
module ScrollViewExample = ScrollView;

open Revery.UI;
open Revery.UI.Components;

let bgColor = Color.rgba(0., 0., 0., 0.);
/* let activeBackgroundColor = Color.rgba("#2E3440"); */
let activeBackgroundColor = bgColor;
let inactiveBackgroundColor = Color.hex("#272d39");
let selectionHighlight = Color.hex("#90f7ff");

type example = {
  name: string,
  render: Window.t => React.syntheticElement,
};

type state = {
  examples: list(example),
  selectedExample: string,
};

let state: state = {
  examples: [
    {name: "Animation", render: _w => Hello.render()},
    {name: "Button", render: _ => DefaultButton.render()},
    {name: "Checkbox", render: _ => CheckboxExample.render()},
    {name: "Slider", render: _ => SliderExample.render()},
    {name: "Border", render: _ => Border.render()},
    {name: "ScrollView", render: _w => ScrollViewExample.render()},
    {name: "Calculator", render: w => Calculator.render(w)},
    {name: "Flexbox", render: _ => Flexbox.render()},
    {name: "Box Shadow", render: _ => Boxshadow.render()},
    {name: "Focus", render: _ => Focus.render()},
    {name: "Stopwatch", render: _ => Stopwatch.render()},
    {name: "Native", render: w => Native.render(w)},
    {name: "Input", render: w => InputExample.render(w)},
    {name: "Radio Button", render: _ => RadioButtonExample.render()},
    {name: "Game Of Life", render: _ => GameOfLife.render()},
    {name: "Screen Capture", render: w => ScreenCapture.render(w)},
    {name: "Tree View", render: w => TreeView.render(w)},
  ],
  selectedExample: "Animation",
};

let noop = () => ();

let getRenderFunctionSelector: (state, Window.t) => React.syntheticElement =
  (s: state) =>
    List.filter(x => String.equal(x.name, s.selectedExample), state.examples)
    |> List.hd
    |> (a => a.render);

module ExampleButton = {
  let component = React.component("ExampleButton");

  let make = (~isActive: bool, ~name, ~onClick, ()) =>
    component((_slots: React.Hooks.empty) => {
      let highlightColor =
        isActive ? selectionHighlight : Colors.transparentWhite;

      let buttonOpacity = 1.0;
      let bgColor = isActive ? activeBackgroundColor : inactiveBackgroundColor;

      let wrapperStyle =
        Style.[
          opacity(buttonOpacity),
          borderLeft(~width=4, ~color=highlightColor),
          backgroundColor(bgColor),
        ];

      let textColor = isActive ? Colors.white : Colors.grey;
      let textHeaderStyle =
        Style.[
          color(textColor),
          fontFamily("Roboto-Regular.ttf"),
          fontSize(14),
          margin(16),
        ];

      <View style=[Style.opacity(buttonOpacity)]>
        <Clickable style=wrapperStyle onClick>
          <Text style=textHeaderStyle text=name />
        </Clickable>
      </View>;
    });

  let createElement = (~children as _, ~isActive, ~name, ~onClick, ()) =>
    React.element(make(~isActive, ~name, ~onClick, ()));
};

type action =
  | SelectExample(string);

let reducer = (s: state, a: action) => {
  switch (a) {
  | SelectExample(name) => {...s, selectedExample: name}
  };
};

let init = app => {
  let maximized = Environment.webGL;

  let win =
    App.createWindow(
      app,
      "Welcome to Revery!",
      ~createOptions={
        ...Window.defaultCreateOptions,
        maximized,
        transparent: true,
        backgroundColor: Color.rgba(0., 0., 0., 0.),
      },
    );

  let render = () => {
    let s = App.getState(app);

    let renderButton = (x: example) => {
      let isActive = String.equal(x.name, s.selectedExample);
      <ExampleButton
        isActive
        name={x.name}
        onClick={_ => {
          /*
           * TEMPORARY WORKAROUND: The animations don't always get stopped when switching examples,
           * tracked by briskml/brisk-reconciler#8. We can remove this once it's fixed!
           */
          Animated.cancelAll();

          App.dispatch(app, SelectExample(x.name));
        }}
      />;
    };

    let buttons = List.map(renderButton, s.examples);

    let exampleRender = getRenderFunctionSelector(s);
    let example = exampleRender(win);

    <View
      onMouseWheel={evt =>
        print_endline("onMouseWheel: " ++ string_of_float(evt.deltaY))
      }
      style=Style.[
        position(`Absolute),
        justifyContent(`Center),
        alignItems(`Center),
        backgroundColor(bgColor),
        bottom(0),
        top(0),
        left(0),
        right(0),
        flexDirection(`Row),
      ]>
      <ScrollView
        style=Style.[
          position(`Absolute),
          top(0),
          left(0),
          width(175),
          bottom(0),
          backgroundColor(bgColor),
        ]>
        <View> ...buttons </View>
      </ScrollView>
      <View
        style=Style.[
          position(`Absolute),
          top(0),
          left(175),
          right(0),
          bottom(0),
          backgroundColor(activeBackgroundColor),
        ]>
        example
      </View>
    </View>;
  };

  if (Environment.webGL) {
    Window.maximize(win);
  };

  UI.start(win, render);
};

App.startWithState(state, reducer, init);