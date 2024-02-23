import { createContext, useContext, useState } from 'react';

const ADD_MESSAGE_COLOR = '#3d84b8';
const REGULAR_MESSAGE_COLOR = '#2b2e4a';
const ERROR_MESSAGE_COLOR = '#fb3640';

const ScoreCardContext = createContext({
  messages: [],
  add_messages: [],
  query_messages: [],

  add_rows: [],
  query_rows: [],

  addCardMessage: () => {},
  addRegularMessage: () => {},
  addErrorMessage: () => {},
  ResetRegularMessage: () => {},
});

const makeMessage = (message, color) => {
  return { message, color };
};

const ScoreCardProvider = (props) => {
  const [value, setValue] = useState(0);
  const [messages, setMessages] = useState([]);
  const [add_messages, setAddMessages] = useState([]);
  const [query_messages, setQueryMessages] = useState([]);

  const [add_rows, setAddRows] = useState([]);
  const [query_rows, setQueryRows] = useState([]);

  const addCardMessage = (message) => {
    setAddMessages([...add_messages, makeMessage(message, ADD_MESSAGE_COLOR)]);
  };

  const addRegularMessage = (...ms) => {
    setQueryMessages([
      ...query_messages,
      ...ms.map((m) => makeMessage(m, REGULAR_MESSAGE_COLOR)),
    ]);
  };

  const ResetRegularMessage = (msg) => {
    setMessages([makeMessage(msg, REGULAR_MESSAGE_COLOR)]);
    setAddMessages([]);
    setQueryMessages([]);
    setAddRows([]);
    setQueryRows([]);
  }

  const addErrorMessage = (message, mode) => {
    if(mode==='add') setAddMessages([...add_messages, makeMessage(message, ERROR_MESSAGE_COLOR)]);
    else if(mode==='query') setQueryMessages([...query_messages, makeMessage(message, ERROR_MESSAGE_COLOR)]);
  };

  return (
    <ScoreCardContext.Provider
      value={{
        value,
        messages,
        add_messages,
        query_messages,
        add_rows,
        query_rows,
        setAddRows,
        setQueryRows,
        setValue,
        addCardMessage,
        addRegularMessage,
        addErrorMessage,
        ResetRegularMessage,
      }}
      {...props}
    />
  );
};

function useScoreCard() {
  return useContext(ScoreCardContext);
}

export { ScoreCardProvider, useScoreCard };
